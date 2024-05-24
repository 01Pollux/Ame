#pragma once

#include <Math/Matrix.hpp>
#include <Math/Vector.hpp>
#include <glm/gtx/norm.hpp>
#include <span>

namespace Ame::Math
{
    struct Plane : Vector4
    {
    public:
        using Vector4::Vector4;

        constexpr Plane() :
            Vector4(0.f, 0.f, 0.f, 0.f)
        {
        }

        Plane(
            const Vector3& point,
            const Vector3& normal) :
            Vector4(normal, -glm::dot(point, normal))
        {
        }

        Plane(
            const Vector3& p0,
            const Vector3& p1,
            const Vector3& p2) :
            Vector4(glm::cross(p1 - p0, p2 - p0), 0.f)
        {
            Normalize();
            w = -glm::dot(AsVec3(), p0);
        }

    public:
        /// <summary>
        /// Construct a plane from a normal and a distance
        /// </summary>
        [[nodiscard]] Vector4& AsVec4() noexcept
        {
            return static_cast<Vector4&>(*this);
        }

        /// <summary>
        /// Construct a plane from a normal and a distance
        /// </summary>
        [[nodiscard]] const Vector4& AsVec4() const noexcept
        {
            return static_cast<const Vector4&>(*this);
        }

        /// <summary>
        /// Construct a plane from a normal
        /// </summary>
        [[nodiscard]] Vector3 AsVec3() const noexcept
        {
            return static_cast<Vector3>(*this);
        }

    public:
        /// <summary>
        /// Construct a plane from a normal and a point
        /// </summary>
        [[nodiscard]] Vector3 GetNormal() const noexcept
        {
            return AsVec3();
        }

        /// <summary>
        /// Get the distance from the origin
        /// </summary>
        [[nodiscard]] float GetDistance() const noexcept
        {
            return w;
        }

        /// <summary>
        /// Flip the plane normal
        /// </summary>
        void Flip()
        {
            AsVec4() = -AsVec4();
        }

    public:
        /// <summary>
        /// Get the distance from the point
        /// </summary>
        [[nodiscard]] float GetDistanceTo(
            const Vector3& point) const
        {
            return glm::dot(GetNormal(), point) + GetDistance();
        }

        /// <summary>
        /// Project point in plane
        /// </summary>
        [[nodiscard]] Vector3 Project(
            const Vector3& point) const
        {
            return point - GetNormal() * GetDistanceTo(point);
        }

    public:
        /// <summary>
        /// Check intersection between plane and point
        /// </summary>
        void IntersectAxisAlignedBox(
            const Vector4& center,
            const Vector3& extents,
            bool&          outside,
            bool&          inside) const
        {
            // Compute the distance to the center of the box.
            float dist = glm::dot(center, AsVec4());

            // Project the axes of the box onto the normal of the plane.  Half the
            // length of the projection (sometime called the "radius") is equal to
            // h(u) * abs(n dot b(u))) + h(v) * abs(n dot b(v)) + h(w) * abs(n dot b(w))
            // where h(i) are extents of the box, n is the plane normal, and b(i) are the
            // axes of the box. In this case b(i) = [(1,0,0), (0,1,0), (0,0,1)].
            float radius = glm::dot(extents, glm::abs(AsVec3()));

            // Outside the plane?
            outside = dist > radius;

            // Fully inside the plane?
            inside = dist < -radius;
        }

        /// <summary>
        /// Check intersection between plane and sphere
        /// </summary>
        void IntersectFrustum(
            std::span<const Vector4> points,
            bool&                    outside,
            bool&                    inside) const
        {
            float min = std::numeric_limits<float>::max(),
                  max = std::numeric_limits<float>::lowest();

            Vector3 vec3 = AsVec3();
            for (auto& point : points)
            {
                float dist = glm::distance(vec3, Vector3(point));
                min        = glm::min(min, dist);
                max        = glm::max(max, dist);
            }

            outside = min > -w;
            inside  = max < -w;
        }

    public:
        /// <summary>
        /// Transform plane by matrix
        /// </summary>
        void Transform(
            const Matrix4x4& m)
        {
            Vector4 r = w * m[3];
            r         = z * m[2] + r;
            r         = y * m[1] + r;
            AsVec4()  = x * m[0] + r;

            w = glm::length(AsVec3());
            AsVec3() /= w;
        }

        /// <summary>
        /// Transform plane by rotation and translation
        /// </summary>
        void Transform(
            const Quaternion& rotation,
            const Vector3&    translation)
        {
            Vector3 normal = rotation * Vector4(AsVec3(), 0.f);
            float   d      = w - glm::dot(normal, translation);
            *this          = Plane(normal, d);
        }

        /// <summary>
        /// Normalize the plane
        /// </summary>
        void Normalize()
        {
            AsVec4() = Vector4(glm::normalize(AsVec3()), w);
        }
    };
} // namespace Ame::Math