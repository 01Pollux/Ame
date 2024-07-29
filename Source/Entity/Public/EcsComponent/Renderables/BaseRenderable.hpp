#pragma once

#include <Core/Serializable.hpp>
#include <Math/Matrix.hpp>
#include <Geometry/AABB.hpp>    

namespace Ame::Ecs
{
    using VI3_Position = Math::Vector3;
    using VI3_Normal   = Math::Vector3;
    using VI3_Tangent  = Math::Vector3;

    using VI2_Position = Math::Vector2;
    using VI2_Normal   = Math::Vector2;
    using VI2_Tangent  = Math::Vector2;

    using VI_TexCoord = Math::Vector2;

    //

    class IBaseRenderable : public ISerializable
    {
    public:
    };

    /// <summary>
    /// Automatically set when a renderable is added to an entity.
    /// the component will be set as pair, where the first pair is the component type, and the second pair is the renderable.
    /// (ComponentType, BaseRenderable)
    /// </summary>
    struct RenderableComponent
    {
        IBaseRenderable* Object;
    };
} // namespace Ame::Ecs