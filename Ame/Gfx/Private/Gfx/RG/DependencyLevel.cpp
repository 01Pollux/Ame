#include <Gfx/RG/DependencyLevel.hpp>
#include <Gfx/RG/Context.hpp>
#include <Gfx/RG/Pass.hpp>
#include <Gfx/RG/Dispatch.hpp>

#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/CommandList/Marker.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    void DependencyLevel::AddPass(
        Context&                                          context,
        Pass*                                             pass,
        std::vector<ResourceViewId>                       renderTargets,
        ResourceViewId                                    depthStencil,
        std::set<ResourceId>                              resourceToCreate,
        const std::map<ResourceViewId, Rhi::AccessStage>& resourceStates,
        const std::map<ResourceId, Rhi::LayoutType>&      textureLayouts)
    {
        m_Passes.emplace_back(RenderPassInfo{
            .NodePass      = std::move(pass),
            .RenderTargets = std::move(renderTargets),
            .DepthStencil  = std::move(depthStencil) });

        m_ResourcesToCreate.merge(std::move(resourceToCreate));

        auto& resourceStorage = context.GetStorage();
        for (auto& [viewId, state] : resourceStates)
        {
            std::visit(
                VariantVisitor{
                    [&](const Rhi::BufferViewDesc& view)
                    {
                        auto& curState = m_BufferStatesToTransitions[viewId.GetResource()];
                        curState.access |= state.access;
                        curState.stages |= state.stages;
                    },
                    [&](const Rhi::TextureViewDesc& view)
                    {
                        auto& curState = m_TextureStatesToTransitions[viewId.GetResource()][view.Subresource];
                        curState.access |= state.access;
                        curState.stages |= state.stages;
                    } },
                resourceStorage.GetResourceViewDesc(viewId));
        }

        for (auto& [id, layout] : textureLayouts)
        {
            auto& curState = m_TextureStatesToTransitions[id];
            for (auto& iter : curState)
            {
                auto& state  = iter.second;
                state.layout = layout;
            }
        }
    }

    //

    void DependencyLevel::Execute(
        Context&          context,
        Rhi::CommandList& commandList) const
    {
        LockStorage(context);

        ExecuteBarriers(context, commandList);
        ExecutePasses(context, commandList);

        UnlockStorage(context);
    }

    //

    void DependencyLevel::LockStorage(
        Context& context) const
    {
        auto& resourceStorage = context.GetStorage();
        resourceStorage.Lock();
    }

    //

    void DependencyLevel::ExecuteBarriers(
        Context&          context,
        Rhi::CommandList& commandList) const
    {
        if (m_BufferStatesToTransitions.empty() &&
            m_TextureStatesToTransitions.empty())
        {
            return;
        }

        auto& resourceStorage = context.GetStorage();

        for (auto& [resource, state] : m_BufferStatesToTransitions)
        {
            auto& buffer = *resourceStorage.GetResource(resource)->AsBuffer();
            commandList.RequireState(buffer, state);
        }

        for (auto& [resource, stateMap] : m_TextureStatesToTransitions)
        {
            auto& texture = *resourceStorage.GetResource(resource)->AsTexture();
            for (auto& [subresourceSet, state] : stateMap)
            {
                commandList.RequireState(texture, state, subresourceSet);
            }
        }

        commandList.CommitBarriers();
    }

    //

    void DependencyLevel::ExecutePasses(
        Context&          context,
        Rhi::CommandList& commandList) const
    {
        using namespace EnumBitOperators;

        auto& resourceStorage = context.GetStorage();

        for (auto& PassInfo : m_Passes)
        {
            Rhi::CommandListMarker marker(commandList, PassInfo.NodePass->GetName().data());

            bool noSetup = (PassInfo.NodePass->GetFlags() & PassFlags::NoSetups) != PassFlags::None;
            switch (PassInfo.NodePass->GetQueueType())
            {
            case PassFlags::Graphics:
            {
                GraphicsSetup RenderSetup(commandList, resourceStorage, !noSetup, PassInfo.RenderTargets, PassInfo.DepthStencil);
                PassInfo.NodePass->DoExecute(resourceStorage, &commandList);

                break;
            }

            case PassFlags::Compute:
            case PassFlags::Transfer:
            {
                PassInfo.NodePass->DoExecute(resourceStorage, &commandList);
                break;
            }

            default:
            {
                PassInfo.NodePass->DoExecute(resourceStorage, nullptr);
                break;
            }
            }
        };
    }

    //

    void DependencyLevel::UnlockStorage(
        Context& context) const
    {
        auto& resourceStorage = context.GetStorage();
        resourceStorage.Unlock();
    }
} // namespace Ame::Gfx::RG