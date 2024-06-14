#pragma once

#include <Ecs/VertexInput.hpp>

namespace Ame::Ecs::Component
{
    struct BaseRenderable
    {
        uint32_t CameraMask = 0xFFFF'FFFF;
    };
} // namespace Ame::Ecs::Component