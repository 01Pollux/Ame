#pragma once

#include <Ecs/Entity.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <Ecs/Component/Math/Transform.hpp>

namespace Ame::Gfx
{
    struct EntityDrawInfo
    {
        Ecs::Entity                           Entity;
        const Ecs::Component::BaseRenderable* Renderable;
        const Ecs::Component::Transform*      Transform;
    };
} // namespace Ame::Gfx