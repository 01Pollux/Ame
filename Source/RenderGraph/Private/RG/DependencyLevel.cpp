#include <Rg/DependencyLevel.hpp>
#include <Rg/Context.hpp>
#include <Rg/Pass.hpp>

#include <DiligentCore/Graphics/GraphicsTools/interface/ScopedDebugGroup.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rg
{
    void DependencyLevel::AddPass(
        Pass*                       pass,
        std::vector<ResourceViewId> renderTargets,
        ResourceViewId              depthStencil,
        std::set<ResourceId>        resourceToCreate)
    {
        m_Passes.emplace_back(RenderPassInfo{
            .NodePass      = std::move(pass),
            .RenderTargets = std::move(renderTargets),
            .DepthStencil  = std::move(depthStencil) });
        m_ResourcesToCreate.merge(std::move(resourceToCreate));
    }

    //

    void DependencyLevel::Execute(
        Context&            context,
        Dg::IDeviceContext* deviceContext) const
    {
        LockStorage(context);
        ExecutePasses(context, deviceContext);
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

    static void SetupForRendering(
        Dg::IDeviceContext*             deviceContext,
        const ResourceStorage&          resourceStorage,
        std::span<const ResourceViewId> renderTargets,
        const ResourceViewId&           depthStencil)
    {
        std::vector<Dg::ITextureView*> renderTargetsViews;
        std::vector<Math::Color4>      clearColors;

        Dg::ITextureView*             depthStencilView  = nullptr;
        float                         depthClearValue   = 0.0f;
        uint8_t                       stencilClearValue = 0;
        Dg::CLEAR_DEPTH_STENCIL_FLAGS depthClearFlags   = Dg::CLEAR_DEPTH_FLAG_NONE;

        clearColors.reserve(renderTargets.size());
        renderTargetsViews.reserve(renderTargets.size());

        for (auto& viewId : renderTargets)
        {
            auto  resource = resourceStorage.GetResource(viewId.GetResource());
            auto& viewDesc = *resource->GetTextureView(viewId);

            auto rtv = std::get_if<RenderTargetViewDesc>(&viewDesc.Desc);
            if (rtv)
            {
                if (rtv->ClearType != ERTClearType::Ignore)
                {
                    auto texture = resource->AsTexture();
                    if (rtv->ForceColor)
                    {
                        clearColors.emplace_back(rtv->ClearColor);
                    }
                    else
                    {
                        auto& clearColor = texture->Desc.ClearValue.Color;
                        clearColors.emplace_back(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
                    }
                }
            }
            renderTargetsViews.emplace_back(viewDesc.View);
        }

        if (depthStencil)
        {
            auto  resource = resourceStorage.GetResource(depthStencil.GetResource());
            auto& viewDesc = *resource->GetTextureView(depthStencil);

            auto dsv         = std::get_if<DepthStencilViewDesc>(&viewDesc.Desc);
            depthStencilView = viewDesc.View;
            if (dsv)
            {
                if (dsv->ClearType != EDSClearType::Ignore)
                {
                    auto& handle            = *resourceStorage.GetResource(depthStencil.GetResource());
                    auto& textureClearValue = handle.AsTexture()->Desc.ClearValue.DepthStencil;

                    switch (dsv->ClearType)
                    {
                    case EDSClearType::Depth:
                    {
                        depthClearFlags = Dg::CLEAR_DEPTH_FLAG;
                        depthClearValue = dsv->ForceDepth ? dsv->Depth : textureClearValue.Depth;

                        break;
                    }
                    case EDSClearType::Stencil:
                    {
                        depthClearFlags   = Dg::CLEAR_STENCIL_FLAG;
                        stencilClearValue = dsv->ForceStencil ? dsv->Stencil : textureClearValue.Stencil;

                        break;
                    }
                    case EDSClearType::DepthStencil:
                    {
                        depthClearFlags   = Dg::CLEAR_DEPTH_FLAG | Dg::CLEAR_STENCIL_FLAG;
                        depthClearValue   = dsv->ForceDepth ? dsv->Depth : textureClearValue.Depth;
                        stencilClearValue = dsv->ForceStencil ? dsv->Stencil : textureClearValue.Stencil;

                        break;
                    }

                    default:
                        std::unreachable();
                    }
                }
            }
        }

        deviceContext->SetRenderTargets(Rhi::Count32(renderTargetsViews), renderTargetsViews.data(), depthStencilView, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        for (auto [view, clearColor] : std::views::zip(renderTargetsViews, clearColors))
        {
            deviceContext->ClearRenderTarget(view, clearColor.data(), Dg::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        }
        if (depthClearFlags)
        {
            deviceContext->ClearDepthStencil(depthStencilView, depthClearFlags, depthClearValue, stencilClearValue, Dg::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        }
    }

    void DependencyLevel::ExecutePasses(
        Context&            context,
        Dg::IDeviceContext* deviceContext) const
    {
        using namespace EnumBitOperators;

        auto& resourceStorage = context.GetStorage();

        for (auto& passInfo : m_Passes)
        {
            Dg::ScopedDebugGroup marker(deviceContext, passInfo.NodePass->GetName().data(), passInfo.NodePass->GetColorPtr());

            bool noSetup = (passInfo.NodePass->GetFlags() & PassFlags::NoSetups) != PassFlags::None;
            switch (passInfo.NodePass->GetQueueType())
            {
            case PassFlags::Graphics:
            {
                if (!noSetup)
                {
                    SetupForRendering(deviceContext, resourceStorage, passInfo.RenderTargets, passInfo.DepthStencil);
                }
                passInfo.NodePass->DoExecute(resourceStorage, deviceContext);
                break;
            }

            case PassFlags::Compute:
            case PassFlags::Copy:
            {
                passInfo.NodePass->DoExecute(resourceStorage, deviceContext);
                break;
            }

            default:
            {
                passInfo.NodePass->DoExecute(resourceStorage, nullptr);
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
} // namespace Ame::Rg