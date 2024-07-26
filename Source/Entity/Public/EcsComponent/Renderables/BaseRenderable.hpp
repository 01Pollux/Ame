#pragma once

#include <Core/Serializable.hpp>

namespace Ame::Ecs
{
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
        IBaseRenderable* cs;
    };
} // namespace Ame::Ecs