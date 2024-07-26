#pragma once

#include <Math/Transform.hpp>

namespace Ame::Ecs
{
    struct TransformComponent final : Math::TransformMatrix
    {
        using TransformMatrix::TransformMatrix;
    };

    /// <summary>
    /// Component set automatically when an entity has a transform component.
    /// </summary>
    struct GlobalTransformComponent final : Math::TransformMatrix
	{
		using TransformMatrix::TransformMatrix;
	};
} // namespace Ame::Ecs::Component