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

namespace Ame::Gfx
{
    class EntityCompositor;
} // namespace Ame::Gfx

namespace Ame::Signals
{
    namespace Data
    {
        struct DrawCompositorData
        {
            Gfx::DrawCompositorSubmitter Compositor;

            Ecs::Entity                     CameraEntity;
            CRef<Ecs::Component::Camera>    CameraComponent;
            CRef<Ecs::Component::Transform> CameraTransform;

            std::span<const Gfx::EntityDrawInfo> Entities;

            [[nodiscard]] float DistanceTo(const Gfx::EntityDrawInfo& drawInfo) const
            {
                auto& cameraPosition = CameraTransform.get().GetPosition();
                auto& entityPosition = drawInfo.Transform.get().GetPosition();
                return cameraPosition.DistanceTo(entityPosition);
            }
        };
    } // namespace Data
    AME_SIGNAL_DECL(OnRenderCompose, void(Data::DrawCompositorData& renderData));
} // namespace Ame::Signals
