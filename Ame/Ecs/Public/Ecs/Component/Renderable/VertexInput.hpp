#pragma once

#include <Math/Vector.hpp>

namespace Ame::Ecs::Component
{
    struct VertexInput
    {
        Math::Vector3 Position;
        Math::Vector3 Normal;
        Math::Vector3 Tangent;
        Math::Vector2 TexCoord;
    };
} // namespace Ame::Ecs::Component