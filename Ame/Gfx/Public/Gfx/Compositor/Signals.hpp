#pragma once

#include <Core/Ame.hpp>
#include <Object/Signal.hpp>

#include <Gfx/Compositor/EntityDrawInfo.hpp>
#include <Gfx/Compositor/DrawCompositorSubmitter.hpp>

namespace Ame
{
    namespace Math
    {
        class Camera;
        class Transform;
    } // namespace Math
    namespace Gfx
    {
        class EntityCompositor;
    } // namespace Gfx
} // namespace Ame

namespace Ame::Signals
{
    namespace Data
    {
        struct DrawCompositorData
        {
            Gfx::DrawCompositorSubmitter Compositor;

            Ecs::Entity                 CameraEntity;
            CRef<Math::Camera>          CameraComponent;
            CRef<Math::TransformMatrix> CameraTransform;

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
