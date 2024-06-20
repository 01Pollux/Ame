#include <RG/Dispatch.hpp>
#include <RG/ResourceStorage.hpp>

namespace Ame::RG
{
    GraphicsSetup::GraphicsSetup(
        Rhi::CommandList&               commandList,
        const ResourceStorage&          resourceStorage,
        bool                            enable,
        std::span<const ResourceViewId> renderTargets,
        const ResourceViewId&           depthStencil)
    {
        if (!enable)
        {
            return;
        }

        m_CommandList = &commandList;
        std::vector<Rhi::ClearDesc> clearDescs(renderTargets.size() + (depthStencil ? 1 : 0));

        auto Rtvs = SetupRenderTargets(resourceStorage, clearDescs, renderTargets);
        auto Dsv  = SetupDepthStencil(resourceStorage, clearDescs, depthStencil);

        m_CommandList->BeginRendering(Rtvs, Dsv);
        if (!clearDescs.empty())
        {
            m_CommandList->ClearAttachments(clearDescs);
        }
    }

    GraphicsSetup::~GraphicsSetup()
    {
        if (m_CommandList)
        {
            m_CommandList->EndRendering();
        }
    }

    //

    std::vector<const Rhi::ResourceView*> GraphicsSetup::SetupRenderTargets(
        const ResourceStorage&          resourceStorage,
        std::span<Rhi::ClearDesc>       clearDescs,
        std::span<const ResourceViewId> renderTargets) const
    {
        std::vector<const Rhi::ResourceView*> rtvs;
        rtvs.reserve(renderTargets.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(renderTargets.size()); i++)
        {
            auto& rtvViewId = renderTargets[i];
            auto  resource  = resourceStorage.GetResource(rtvViewId.GetResource());

            auto& view    = *resource->GetTextureView(rtvViewId);
            auto& rtvDesc = std::get<RenderTargetViewDesc>(view.Desc);

            rtvs.push_back(resourceStorage.GetResourceViewHandle(rtvViewId));
            if (rtvDesc.ClearType != ERTClearType::Ignore)
            {
                auto& handle  = *resourceStorage.GetResource(rtvViewId.GetResource());
                auto  texture = handle.AsTexture();

                clearDescs[i].attachmentContentType = Rhi::AttachmentContentType::COLOR;
                clearDescs[i].colorAttachmentIndex  = i;

                auto& clearColor = clearDescs[i].value.color32f;
                if (rtvDesc.ForceColor)
                {
                    clearColor.x = rtvDesc.ClearColor.r();
                    clearColor.y = rtvDesc.ClearColor.g();
                    clearColor.z = rtvDesc.ClearColor.b();
                    clearColor.w = rtvDesc.ClearColor.a();
                }
                else
                {
                    clearColor.x = clearColor.y = clearColor.z = clearColor.w = 0.f;
                }
            }
        }

        return rtvs;
    }

    const Rhi::ResourceView* GraphicsSetup::SetupDepthStencil(
        const ResourceStorage&    resourceStorage,
        std::span<Rhi::ClearDesc> clearDescs,
        const ResourceViewId&     depthStencil) const
    {
        if (!depthStencil)
        {
            return nullptr;
        }

        auto resource = resourceStorage.GetResource(depthStencil.GetResource());

        auto& view    = *resource->GetTextureView(depthStencil);
        auto& dsvDesc = std::get<DepthStencilViewDesc>(view.Desc);

        auto dsv = resourceStorage.GetResourceViewHandle(depthStencil);
        if (dsvDesc.ClearType != EDSClearType::Ignore)
        {
            auto& handle    = *resourceStorage.GetResource(depthStencil.GetResource());
            auto& clearDesc = clearDescs[clearDescs.size() - 1];
            switch (dsvDesc.ClearType)
            {
            case EDSClearType::Depth:
            {
                clearDesc.value.depthStencil.depth = dsvDesc.Depth;
                clearDesc.attachmentContentType    = Rhi::AttachmentContentType::DEPTH;

                break;
            }
            case EDSClearType::Stencil:
            {
                clearDesc.value.depthStencil.stencil = dsvDesc.Stencil;
                clearDesc.attachmentContentType      = Rhi::AttachmentContentType::STENCIL;

                break;
            }
            case EDSClearType::DepthStencil:
            {
                clearDesc.value.depthStencil.depth   = dsvDesc.Depth;
                clearDesc.value.depthStencil.stencil = dsvDesc.Stencil;
                clearDesc.attachmentContentType      = Rhi::AttachmentContentType::DEPTH_STENCIL;

                break;
            }

            default:
                std::unreachable();
            }
        }

        return dsv;
    }
} // namespace Ame::RG