#include <Geometry/AABB.hpp>
#include <Geometry/Frustum.hpp>

namespace Ame::Geometry
{
    void AABB::Transform(
        const Math::Vector3&    scale,
        const Math::Quaternion& rotation,
        const Math::Vector3&    translation)
    {
        Math::Vector3 min(std::numeric_limits<float>::max()),
            max(std::numeric_limits<float>::lowest());

        // Compute and transform the corners and find new min/max bounds.
        for (size_t i = 0; i < 8; ++i)
        {
            Math::Vector3 corner = Extents * c_BoxOffset[i] + Center;
            corner               = glm::rotate(rotation, corner * scale) + translation;

            min = glm::min(min, corner);
            max = glm::max(max, corner);
        }

        Center  = (min + max) * 0.5f;
        Extents = (max - min) * 0.5f;
    }

    //

    ContainmentType AABB::Contains(
        const AABB& box) const
    {
        Math::Vector3 minA = Min();
        Math::Vector3 maxA = Max();

        Math::Vector3 minB = box.Min();
        Math::Vector3 maxB = box.Max();

        // for each i in (x, y, z) if a_min(i) > b_max(i) or b_min(i) > a_max(i) then return false
        bool disjoint = glm::any(glm::greaterThan(minA, maxB) | glm::greaterThan(minB, maxA));
        if (disjoint)
        {
            return ContainmentType::Disjoint;
        }

        // for each i in (x, y, z) if a_min(i) <= b_min(i) and b_max(i) <= a_max(i) then A contains B
        bool inside = glm::all(glm::lessThanEqual(minA, minB) && glm::lessThanEqual(maxB, maxA));
        return inside ? ContainmentType::Contains : ContainmentType::Intersects;
    }

    ContainmentType AABB::Contains(
        const Frustum& fr) const
    {
        return fr.Contains(*this);
    }

    ContainmentType AABB::Contains(
        std::span<const Math::Plane> planes) const
    {
        // Set w of the center to one so we can dot4 with a plane.
        Math::Vector4 centerW1(Center, 1.f);

        bool anyOutside = false, allInside = false;
        planes[0].IntersectAxisAlignedBox(centerW1, Extents, anyOutside, allInside);

        for (size_t i = 1; i < planes.size(); i++)
        {
            bool curAnyOutside, curAllInside;
            planes[i].IntersectAxisAlignedBox(centerW1, Extents, curAnyOutside, curAllInside);

            anyOutside |= curAnyOutside;
            allInside &= curAllInside;
        }

        return anyOutside  ? ContainmentType::Disjoint
               : allInside ? ContainmentType::Contains
                           : ContainmentType::Intersects;
    }
} // namespace Ame::Geometry