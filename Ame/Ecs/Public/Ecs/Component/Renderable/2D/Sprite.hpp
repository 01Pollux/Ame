#pragma once

#include <Math/Colors.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Ecs::Component
{
    struct Sprite final : public Component::BaseRenderable
    {
        Math::Color4  ModulationColor = Colors::White;
        Math::Vector2 UVStart         = Math::Vector2(0.0f, 0.0f);
        Math::Vector2 UVEnd           = Math::Vector2(1.0f, 1.0f);
    };
} // namespace Ame::Ecs::Component