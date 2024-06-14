#pragma once

#include <Core/Ame.hpp>
#include <Object/Signal.hpp>

#include <Gfx/Compositor/EntityDrawInfo.hpp>
#include <Gfx/Compositor/DrawCompositorSubmitter.hpp>

namespace Ame::Ecs::Component
{
    class Camera;
    class Transform;
} // namespace Ame::Ecs::Component

namespace Ame::Signals::Data
{
    struct DrawCompositorData
    {
        Gfx::DrawCompositorSubmitter Compositor;

        Ecs::Entity                     CameraEntity;
        CRef<Ecs::Component::Camera>    CameraComponent;
        CRef<Ecs::Component::Transform> CameraTransform;

        std::span<Gfx::EntityDrawInfo> Entities;
    };
} // namespace Ame::Signals::Data

AME_SIGNAL_DECL(OnRenderCompose, void(Data::DrawCompositorData& renderData));
