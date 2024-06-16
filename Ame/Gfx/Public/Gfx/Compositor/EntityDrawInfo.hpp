#pragma once

#include <Ecs/Entity.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <Ecs/Component/Math/Transform.hpp>

namespace Ame::Gfx
{
    struct EntityDrawInfo
    {
        Ecs::Entity                          Entity;
        CRef<Ecs::Component::BaseRenderable> Renderable;
        CRef<Ecs::Component::Transform>      Transform;
    };
} // namespace Ame::Gfx