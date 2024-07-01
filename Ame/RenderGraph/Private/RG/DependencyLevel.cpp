#include <RG/DependencyLevel.hpp>
#include <RG/Context.hpp>
#include <RG/Pass.hpp>
#include <RG/Dispatch.hpp>

#include <Rhi/Device/Device.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
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
            auto& resource = *resourceStorage.GetResource(viewId.GetResource());
            std::visit(
                VariantVisitor{
                    [](std::monostate) {},
                    [&](const BufferResource&)
                    {
                        auto& curState = m_BufferStatesToTransitions[viewId.GetResource()];
                        curState.access |= state.access;
                        curState.stages |= state.stages;
                    },
                    [&](const TextureResource& texture)
                    {
                        auto view = std::get<RhiTextureViewRef>(resourceStorage.GetResourceView(viewId));
                        std::visit(
                            VariantVisitor{
                                [&](const auto& viewDesc)
                                {
                                    auto& curState = m_TextureStatesToTransitions[viewId.GetResource()][viewDesc.Subresource];
                                    curState.access |= state.access;
                                    curState.stages |= state.stages;
                                } },
                            view.get().Desc);
                    } },
                resource.Get());
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
        auto& stateTracker    = resourceStorage.GetStateTracker();

        auto& rhiDevice  = resourceStorage.GetDevice();
        auto& deviceDesc = rhiDevice.GetDesc();

        for (auto& [resourceId, state] : m_BufferStatesToTransitions)
        {
            auto& resource = *resourceStorage.GetResourceMut(resourceId);
            auto& buffer   = resource.AsBuffer()->Resource;

            stateTracker.RequireState(resource.AsBuffer()->Resource, state);
        }

        for (auto& [resourceId, stateMap] : m_TextureStatesToTransitions)
        {
            auto& resource = *resourceStorage.GetResource(resourceId);
            auto  texture  = resource.AsTexture()->Resource;

            for (auto& [subresourceSet, state] : stateMap)
            {
                stateTracker.RequireState(texture, state, subresourceSet);
            }
        }

        stateTracker.CommitBarriers(commandList);
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
            Rhi::MarkerCommand marker(commandList, PassInfo.NodePass->GetName().data());

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
            case PassFlags::Copy:
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
} // namespace Ame::RG