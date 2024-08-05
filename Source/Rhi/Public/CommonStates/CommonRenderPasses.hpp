#pragma once

#include <CommonStates/CommonRenderPasses/BlitRenderPass.hpp>

namespace Ame::Rhi
{
    // {0CF6184B-E2F9-4435-B2A1-794EFEF70F20}
    static constexpr UId IID_CommandRenderPass{ 0xcf6184b, 0xe2f9, 0x4435, { 0xb2, 0xa1, 0x79, 0x4e, 0xfe, 0xf7, 0xf, 0x20 } };

    /// <summary>
    /// All common render passes can use cached render pass.
    /// </summary>s
    class CommonRenderPass : public BaseObject<IObject>, public BlitRenderPass
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_CommandRenderPass, Base);

    public:
        CommonRenderPass(
            IReferenceCounters* counters,
            Rhi::IRhiDevice*    rhiDevice) :
            Base(counters),
            BlitRenderPass(rhiDevice)
        {
        }
    };
} // namespace Ame::Rhi