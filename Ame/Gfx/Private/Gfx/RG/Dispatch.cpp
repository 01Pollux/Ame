#include <Gfx/RG/Dispatch.hpp>
#include <Gfx/RG/Storage.hpp>

namespace Ame::Gfx::RG
{
    GraphicsSetup::GraphicsSetup(
        Rhi::CommandList&               CommandList,
        const Storage&                  RgStorage,
        bool                            Enable,
        std::span<const ResourceViewId> RenderTargets,
        const ResourceViewId&           DepthStencil)
    {
        if (!Enable)
        {
            return;
        }

        m_CommandList = &CommandList;
        std::vector<Rhi::ClearDesc> ClearDescs(RenderTargets.size() + (DepthStencil ? 1 : 0));

        auto Rtvs = SetupRenderTargets(RgStorage, ClearDescs, RenderTargets);
        auto Dsv  = SetupDepthStencil(RgStorage, ClearDescs, DepthStencil);

        m_CommandList->BeginRendering(Rtvs, Dsv);
        if (!ClearDescs.empty())
        {
            m_CommandList->ClearAttachments(ClearDescs);
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
        const Storage&                  RgStorage,
        std::span<Rhi::ClearDesc>       ClearDescs,
        std::span<const ResourceViewId> RenderTargets) const
    {
        std::vector<const Rhi::ResourceView*> Rtvs;
        Rtvs.reserve(RenderTargets.size());

        for (uint32_t i = 0; i < static_cast<uint32_t>(RenderTargets.size()); i++)
        {
            auto& RtvViewId = RenderTargets[i];
            auto& RtvDesc   = std::get<RenderTargetViewDesc>(RgStorage.GetResourceView(RtvViewId));

            Rtvs.push_back(&RgStorage.GetResourceViewHandle(RtvViewId));
            if (RtvDesc.ClearType != ERTClearType::Ignore)
            {
                auto& Handle  = RgStorage.GetResource(RtvViewId.GetResource());
                auto  Texture = Handle.AsTexture();

                ClearDescs[i].attachmentContentType = nri::AttachmentContentType::COLOR;
                ClearDescs[i].colorAttachmentIndex  = i;
                nri::Color32f& ClearColor           = ClearDescs[i].value.color32f;
                if (RtvDesc.ForceColor)
                {
                    ClearColor.x = RtvDesc.ClearColor.r;
                    ClearColor.y = RtvDesc.ClearColor.g;
                    ClearColor.z = RtvDesc.ClearColor.b;
                    ClearColor.w = RtvDesc.ClearColor.a;
                }
                else
                {
                    ClearColor.x = ClearColor.y = ClearColor.z = ClearColor.w = 0.f;
                }
            }
        }

        return Rtvs;
    }

    const Rhi::ResourceView* GraphicsSetup::SetupDepthStencil(
        const Storage&            RgStorage,
        std::span<Rhi::ClearDesc> ClearDescs,
        const ResourceViewId&     DepthStencil) const
    {
        if (!DepthStencil)
        {
            return nullptr;
        }

        auto& DsvDesc = std::get<DepthStencilViewDesc>(RgStorage.GetResourceView(DepthStencil));
        auto  Dsv     = &RgStorage.GetResourceViewHandle(DepthStencil);
        if (DsvDesc.ClearType != EDSClearType::Ignore)
        {

            auto& Handle    = RgStorage.GetResource(DepthStencil.GetResource());
            auto& ClearDesc = ClearDescs[ClearDescs.size() - 1];
            switch (DsvDesc.ClearType)
            {
            case EDSClearType::Depth:
            {
                ClearDesc.value.depthStencil.depth = DsvDesc.Depth;
                ClearDesc.attachmentContentType    = nri::AttachmentContentType::DEPTH;

                break;
            }
            case EDSClearType::Stencil:
            {
                ClearDesc.value.depthStencil.stencil = DsvDesc.Stencil;
                ClearDesc.attachmentContentType      = nri::AttachmentContentType::STENCIL;

                break;
            }
            case EDSClearType::DepthStencil:
            {
                ClearDesc.value.depthStencil.depth   = DsvDesc.Depth;
                ClearDesc.value.depthStencil.stencil = DsvDesc.Stencil;
                ClearDesc.attachmentContentType      = nri::AttachmentContentType::DEPTH_STENCIL;

                break;
            }

            default:
                std::unreachable();
            }
        }

        return Dsv;
    }
} // namespace Ame::Gfx::RG