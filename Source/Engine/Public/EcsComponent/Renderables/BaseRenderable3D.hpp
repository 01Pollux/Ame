#pragma once

#include <EcsComponent/Renderables/BaseRenderable.hpp>

namespace Ame::Ecs
{
    class IBaseRenderable3D : public IBaseRenderable
    {
    public:
        using IBaseRenderable::IBaseRenderable;
    };

    /// <summary>
    /// Automatically set when a renderable is added to an entity.
    /// </summary>
    AME_ECS_WRAP_COMPONENT_REF(IBaseRenderable3D);
} // namespace Ame::Ecs