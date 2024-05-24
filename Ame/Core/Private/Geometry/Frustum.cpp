#include <Geometry/AABB.hpp>
#include <Geometry/Frustum.hpp>

namespace Ame::Geometry
{
    std::array<Math::Plane, 6> Frustum::GetPlanes() const
    {
        // Create 6 planes
        std::array planes{
            Math::Plane(0.0f, 0.0f, -1.0f, Near),
            Math::Plane(0.0f, 0.0f, 1.0f, -Far),
            Math::Plane(1.0f, 0.0f, -RightSlope, 0.0f),
            Math::Plane(-1.0f, 0.0f, LeftSlope, 0.0f),
            Math::Plane(0.0f, 1.0f, -TopSlope, 0.0f),
            Math::Plane(0.0f, -1.0f, BottomSlope, 0.0f)
        };
        for (auto& plane : planes)
        {
            plane.Transform(Orientation, Origin);
            plane.Normalize();
        }
        return planes;
    }

    //

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

    //

    ContainmentType Frustum::Contains(
        const AABB& box) const
    {
        return box.Contains(GetPlanes());
    }

    ContainmentType Frustum::Contains(
        const Frustum& fr) const
    {
        return fr.Contains(GetPlanes());
    }

    ContainmentType Frustum::Contains(
        std::span<const Math::Plane> planes) const
    {
        // Set w of the origin to one so we can dot4 with a plane.
        Math::Vector4 originW1(Origin, 1.f);

        // Build the corners of the frustum (in world space).
        Math::Vector4 rightTop(RightSlope, TopSlope, 1.0f, 0.0f);
        Math::Vector4 rightBottom(RightSlope, BottomSlope, 1.0f, 0.0f);
        Math::Vector4 leftTop(LeftSlope, TopSlope, 1.0f, 0.0f);
        Math::Vector4 leftBottom(LeftSlope, BottomSlope, 1.0f, 0.0f);

        rightTop    = glm::rotate(Orientation, rightTop);
        rightBottom = glm::rotate(Orientation, rightBottom);
        leftTop     = glm::rotate(Orientation, leftTop);
        leftBottom  = glm::rotate(Orientation, leftBottom);

        std::array corners{
            rightTop * Near + originW1,
            rightBottom * Near + originW1,
            leftTop * Near + originW1,
            leftBottom * Near + originW1,
            rightTop * Far + originW1,
            rightBottom * Far + originW1,
            leftTop * Far + originW1,
            leftBottom * Far + originW1
        };

        bool anyOutside = false, allInside = false;
        planes[0].IntersectFrustum(corners, anyOutside, allInside);

        for (size_t i = 1; i < planes.size(); i++)
        {
            bool curAnyOutside, curAllInside;
            planes[i].IntersectFrustum(corners, curAnyOutside, curAllInside);

            anyOutside |= curAnyOutside;
            allInside &= curAllInside;
        }

        return anyOutside  ? ContainmentType::Disjoint
               : allInside ? ContainmentType::Contains
                           : ContainmentType::Intersects;
    }
} // namespace Ame::Geometry