#pragma once

#include <Gfx/Cache/CommonRenderPass/BlitRenderPass.hpp>

namespace Ame::Gfx::Cache
{
    class CommonRenderPass : public BlitRenderPass
    {
    public:
        CommonRenderPass(
            Rhi::Device& rhiDevice) :
            BlitRenderPass(rhiDevice)
        {
        }
    };
} // namespace Ame::Gfx::Cache