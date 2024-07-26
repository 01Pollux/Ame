#pragma once

#include <EcsComponent/Renderables/BaseRenderable.hpp>

namespace Ame::Ecs
{
    class IBaseRenderable3D : public IBaseRenderable
    {
    public:
    };

    /// <summary>
    /// Automatically set when a renderable is added to an entity.
    /// </summary>
    struct Renderable3DComponent
    {
        IBaseRenderable3D* Object;
    };
} // namespace Ame::Ecs