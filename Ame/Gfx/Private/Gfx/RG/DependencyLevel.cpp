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
        Context&                                          RgContext,
        Pass*                                             RgPass,
        std::vector<ResourceViewId>                       RenderTargets,
        ResourceViewId                                    DepthStencil,
        std::set<ResourceId>                              ResourceToCreate,
        const std::map<ResourceViewId, Rhi::AccessStage>& ResourceStates,
        const std::map<ResourceId, Rhi::LayoutType>&      TextureLayouts)
    {
        m_Passes.emplace_back(RenderPassInfo{
            .RgPass        = std::move(RgPass),
            .RenderTargets = std::move(RenderTargets),
            .DepthStencil  = std::move(DepthStencil) });

        m_ResourcesToCreate.merge(std::move(ResourceToCreate));

        auto& Storage = RgContext.GetStorage();

        for (auto& [ViewId, State] : ResourceStates)
        {
            std::visit(
                VariantVisitor{
                    [&](const Rhi::BufferViewDesc& View)
                    {
                        auto& CurState = m_BufferStatesToTransitions[ViewId.GetResource()];
                        CurState.access |= State.access;
                        CurState.stages |= State.stages;
                    },
                    [&](const Rhi::TextureViewDesc& View)
                    {
                        auto& CurState = m_TextureStatesToTransitions[ViewId.GetResource()][View.Subresource];
                        CurState.access |= State.access;
                        CurState.stages |= State.stages;
                    } },
                Storage.GetResourceViewDesc(ViewId));
        }

        for (auto& [ResourceId, Layout] : TextureLayouts)
        {
            auto& CurState = m_TextureStatesToTransitions[ResourceId];
            for (auto& Iter : CurState)
            {
                auto& State  = Iter.second;
                State.layout = Layout;
            }
        }
    }

    //

    void DependencyLevel::Execute(
        Context&          RgContext,
        Rhi::CommandList& CommandList) const
    {
        LockStorage(RgContext);

        ExecuteBarriers(RgContext, CommandList);
        ExecutePasses(RgContext, CommandList);

        UnlockStorage(RgContext);
    }

    //

    void DependencyLevel::LockStorage(
        Context& RgContext) const
    {
        auto& RgStorage = RgContext.GetStorage();
        RgStorage.Lock();
    }

    //

    void DependencyLevel::ExecuteBarriers(
        Context&          RgContext,
        Rhi::CommandList& CommandList) const
    {
        if (m_BufferStatesToTransitions.empty() &&
            m_TextureStatesToTransitions.empty())
        {
            return;
        }

        auto& RgStorage = RgContext.GetStorage();

        for (auto& [Resource, State] : m_BufferStatesToTransitions)
        {
            auto&       Handle = RgStorage.GetResource(Resource);
            const auto& Buffer = *Handle.AsBuffer();

            CommandList.RequireState(Buffer, State);
        }

        for (auto& [Resource, StateMap] : m_TextureStatesToTransitions)
        {
            auto&       Handle  = RgStorage.GetResource(Resource);
            const auto& Texture = *Handle.AsTexture();

            for (auto& [SubresourceSet, State] : StateMap)
            {
                CommandList.RequireState(Texture, State, SubresourceSet);
            }
        }

        CommandList.CommitBarriers();
    }

    //

    void DependencyLevel::ExecutePasses(
        Context&          RgContext,
        Rhi::CommandList& CommandList) const
    {
        using namespace EnumBitOperators;

        auto& RgStorage = RgContext.GetStorage();

        for (auto& PassInfo : m_Passes)
        {
            Rhi::CommandListMarker PassMarker(CommandList, PassInfo.RgPass->GetName().data());

            bool NoSetup = (PassInfo.RgPass->GetFlags() & PassFlags::NoSetups) != PassFlags::None;
            switch (PassInfo.RgPass->GetQueueType())
            {
            case PassFlags::Graphics:
            {
                GraphicsSetup RenderSetup(CommandList, RgStorage, !NoSetup, PassInfo.RenderTargets, PassInfo.DepthStencil);
                PassInfo.RgPass->DoExecute(RgStorage, &CommandList);

                break;
            }

            case PassFlags::Compute:
            case PassFlags::Transfer:
            {
                PassInfo.RgPass->DoExecute(RgStorage, &CommandList);
                break;
            }

            default:
            {
                PassInfo.RgPass->DoExecute(RgStorage, nullptr);
                break;
            }
            }
        };
    }

    //

    void DependencyLevel::UnlockStorage(
        Context& RgContext) const
    {
        auto& RgStorage = RgContext.GetStorage();
        RgStorage.Unlock();
    }
} // namespace Ame::Gfx::RG