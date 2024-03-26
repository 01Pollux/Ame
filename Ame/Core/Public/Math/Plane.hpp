#pragma once

#include <Math/Vector.hpp>
#include <Math/Matrix.hpp>
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
            const Vector3& Point,
            const Vector3& Normal) :
            Vector4(Normal, -glm::dot(Point, Normal))
        {
        }

        Plane(
            const Vector3& P0,
            const Vector3& P1,
            const Vector3& P2) :
            Vector4(glm::cross(P1 - P0, P2 - P0), 0.f)
        {
            Normalize();
            w = -glm::dot(AsVec3(), P0);
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
            const Vector3& Point) const
        {
            return glm::dot(GetNormal(), Point) + GetDistance();
        }

        /// <summary>
        /// Project point in plane
        /// </summary>
        [[nodiscard]] Vector3 Project(
            const Vector3& Point) const
        {
            return Point - GetNormal() * GetDistanceTo(Point);
        }

    public:
        /// <summary>
        /// Check intersection between plane and point
        /// </summary>
        void IntersectAxisAlignedBox(
            const Vector4& Center,
            const Vector3& Extents,
            bool&          Outside,
            bool&          Inside) const
        {
            // Compute the distance to the center of the box.
            float Dist = glm::dot(Center, AsVec4());

            // Project the axes of the box onto the normal of the plane.  Half the
            // length of the projection (sometime called the "radius") is equal to
            // h(u) * abs(n dot b(u))) + h(v) * abs(n dot b(v)) + h(w) * abs(n dot b(w))
            // where h(i) are extents of the box, n is the plane normal, and b(i) are the
            // axes of the box. In this case b(i) = [(1,0,0), (0,1,0), (0,0,1)].
            float Radius = glm::dot(Extents, glm::abs(AsVec3()));

            // Outside the plane?
            Outside = Dist > Radius;

            // Fully inside the plane?
            Inside = Dist < -Radius;
        }

        /// <summary>
        /// Check intersection between plane and sphere
        /// </summary>
        void IntersectFrustum(
            std::span<const Vector4> Points,
            bool&                    Outside,
            bool&                    Inside) const
        {
            float Min = std::numeric_limits<float>::max(),
                  Max = std::numeric_limits<float>::lowest();

            Vector3 Vec3 = AsVec3();
            for (auto& Point : Points)
            {
                float Dist = glm::distance(Vec3, Vector3(Point));
                Min        = glm::min(Min, Dist);
                Max        = glm::max(Max, Dist);
            }

            Outside = Min > -w;
            Inside  = Max < -w;
        }

    public:
        /// <summary>
        /// Transform plane by matrix
        /// </summary>
        void Transform(
            const Matrix4x4& M)
        {
            Vector4 R = w * M[3];
            R         = z * M[2] + R;
            R         = y * M[1] + R;
            AsVec4()  = x * M[0] + R;

            w = glm::length(AsVec3());
            AsVec3() /= w;
        }

        /// <summary>
        /// Transform plane by rotation and translation
        /// </summary>
        void Transform(
            const Quaternion& Rotation,
            const Vector3&    Translation)
        {
            Vector3 Normal = Rotation * Vector4(AsVec3(), 0.f);
            float   D      = w - glm::dot(Normal, Translation);
            *this          = Plane(Normal, D);
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