#pragma once

#include <Math/Transform.hpp>

namespace Ame::Ecs::Component
{
    struct Transform final : Math::TransformMatrix
    {
        using TransformMatrix::TransformMatrix;
    };
} // namespace Ame::Ecs::Component