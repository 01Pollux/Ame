#include <Geometry/AABB.hpp>
#include <Geometry/Frustum.hpp>

namespace Ame::Geometry
{
    FrustumPlanes Frustum::GetPlanes() const
    {
        auto transformAndNormalize = [this](Math::Plane plane)
        {
            plane.Transform(Orientation, Origin);
            plane.Normalize();
            return plane;
        };
        return FrustumPlanes{
            .Near   = transformAndNormalize(Math::Plane(0.0f, 0.0f, -1.0f, Near)),
            .Far    = transformAndNormalize(Math::Plane(0.0f, 0.0f, 1.0f, -Far)),
            .Right  = transformAndNormalize(Math::Plane(1.0f, 0.0f, -RightSlope, 0.0f)),
            .Left   = transformAndNormalize(Math::Plane(-1.0f, 0.0f, LeftSlope, 0.0f)),
            .Top    = transformAndNormalize(Math::Plane(0.0f, 1.0f, -TopSlope, 0.0f)),
            .Bottom = transformAndNormalize(Math::Plane(0.0f, -1.0f, BottomSlope, 0.0f))
        };
    }

    void Frustum::Transform(
        const Math::Vector3&    scale,
        const Math::Quaternion& rotation,
        const Math::Vector3&    translation)
    {
        // Composite the frustum rotation and the transform rotation.
        Orientation = Orientation * rotation;

        // Transform the origin.
        Origin = glm::rotate(rotation, Origin * scale) + translation;

        float l = glm::length(scale);

        // Scale the near and far distances (the slopes remain the same).
        Near = Near * l;
        Far  = Far * l;
    }
} // namespace Ame::Geometry