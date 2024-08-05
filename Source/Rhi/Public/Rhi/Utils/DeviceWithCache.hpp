#pragma once

#include <Rhi/Device/RhiDevice.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/RenderStateCache.hpp>

namespace Ame::Rhi
{
    template<bool WithError = true>
    class RenderDeviceWithCache : public Dg::RenderDeviceWithCache<WithError>
    {
    public:
        using Base = Dg::RenderDeviceWithCache<WithError>;

        using Base::Base;

        RenderDeviceWithCache(
            IRhiDevice* renderDevice) :
            Base(renderDevice->GetRenderDevice(), GetRenderStateCache(renderDevice))
        {
        }

    private:
        [[nodiscard]] Ptr<Dg::IRenderStateCache> GetRenderStateCache(
            IRhiDevice* renderDevice)
        {
            Ptr<Dg::IRenderStateCache> renderStateCache;
            renderDevice->QueryInterface(Dg::IID_RenderStateCache, renderStateCache.DblPtr<Dg::IObject>());
            return renderStateCache;
        }
    };
} // namespace Ame::Rhi