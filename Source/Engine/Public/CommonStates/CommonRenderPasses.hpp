#pragma once

#include <CommonStates/CommonRenderPasses/BlitRenderPass.hpp>
#include <CommonStates/CommonRenderPasses/CommonTextures.hpp>

namespace Ame::Rhi
{
    // {0CF6184B-E2F9-4435-B2A1-794EFEF70F20}
    inline constexpr UId IID_CommandRenderPass{
        0xcf6184b, 0xe2f9, 0x4435, { 0xb2, 0xa1, 0x79, 0x4e, 0xfe, 0xf7, 0xf, 0x20 }
    };

    /// <summary>
    /// All common render passes can use cached render pass.
    /// </summary>s
    class AME_ENGINE_API CommonRenderPass : public IObjectWithCallback, public BlitRenderPass, public CommonTextures
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CommandRenderPass, IObjectWithCallback);

    private:
        IMPLEMENT_INTERFACE_CTOR(CommonRenderPass, Rhi::IRhiDevice* rhiDevice) :
            IObjectWithCallback(counters), BlitRenderPass(rhiDevice), CommonTextures(rhiDevice)
        {
        }
    };
} // namespace Ame::Rhi