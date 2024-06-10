#pragma once

#include <Math/Vector.hpp>
#include <Math/Util.hpp>

#include <span>

namespace Ame::Math
{
    class Plane : public Vector4
    {
    public:
        using Vector4::Vector4;

        Plane(
            const Vector4& vector) :
            Vector4(vector)
        {
        }

        Plane(
            const Vector3& normal,
            float          distance);

        Plane(
            const Vector3& point,
            Vector3        normal);

        Plane(
            const Vector3& a,
            const Vector3& b,
            const Vector3& c);

    public:
        void Normalize();
        void NormalizeEst();

        [[nodiscard]] Vector3 GetNormal() const;
        [[nodiscard]] float   GetDistance() const;

        [[nodiscard]] std::pair<Vector3, float> GetNormalDistance();

        /// <summary>
        /// Get the intersection point of a line and the plane
        /// </summary>
        [[nodiscard]] Opt<Vector3> IntersectLine(const Vector3& a, const Vector3& b) const;

        /// <summary>
        /// Get the intersection line of two planes
        /// the line is NAN if the planes are parallel
        /// </summary>
        [[nodiscard]] Opt<std::pair<Vector3, Vector3>> IntersectPlane(
            const Plane& other) const;
    };
} // namespace Ame::Math