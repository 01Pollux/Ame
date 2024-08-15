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
            Base(renderDevice->GetRenderDevice(), renderDevice->GetRenderStateCache())
        {
        }
    };
} // namespace Ame::Rhi