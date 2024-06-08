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
} // namespace Ame::Geometry