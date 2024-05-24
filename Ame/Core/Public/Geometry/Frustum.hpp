#pragma once

#include <Geometry/Common.hpp>
#include <array>

namespace Ame::Geometry
{
    struct Frustum
    {
        /// <summary>
        /// Quaternion representing rotation.
        /// </summary>
        Math::Quaternion Orientation = Math::Vec::c_Identity<Math::Quaternion>;

        /// <summary>
        /// Origin of the frustum (and projection).
        /// </summary>
        Math::Vector3 Origin = Math::Vec::c_Zero<Math::Vector3>;

        /// <summary>
        /// Positive X (X/Z)
        /// </summary>
        float RightSlope = 1.f;
        /// <summary>
        /// Negative X
        /// </summary>
        float LeftSlope = -1.f;

        /// <summary>
        /// Positive Y (Y/Z)
        /// </summary>
        float TopSlope = 1.f;
        /// <summary>
        /// Negative Y
        /// </summary>
        float BottomSlope = -1.f;

        /// <summary>
        /// Z of the near plane.
        /// </summary>
        float Near = 0.f;
        /// <summary>
        /// Z of the far plane.
        /// </summary>
        float Far = 1.f;

        Frustum() = default;

        Frustum(
            const Math::Matrix4x4& invProjection)
        {
            // Corners of the projection frustum in homogenous space.
            constexpr Math::Vector4 c_HomogenousPoints[6]{
                { 1.0f, 0.0f, 1.0f, 1.0f },  // right (at far plane)
                { -1.0f, 0.0f, 1.0f, 1.0f }, // left
                { 0.0f, 1.0f, 1.0f, 1.0f },  // top
                { 0.0f, -1.0f, 1.0f, 1.0f }, // bottom
                { 0.0f, 0.0f, 0.0f, 1.0f },  // near
                { 0.0f, 0.0f, 1.0f, 1.0f }   // far
            };

            // Compute the frustum corners in world space.
            Math::Vector4 points[6]{
                invProjection * c_HomogenousPoints[0],
                invProjection * c_HomogenousPoints[1],
                invProjection * c_HomogenousPoints[2],
                invProjection * c_HomogenousPoints[3],
                invProjection * c_HomogenousPoints[4],
                invProjection * c_HomogenousPoints[5]
            };

            // Compute the slopes.
            RightSlope  = points[0].x / points[0].z;
            LeftSlope   = points[1].x / points[1].z;
            TopSlope    = points[2].y / points[2].z;
            BottomSlope = points[3].y / points[3].z;

            // Compute near and far.
            Near = points[4].z / points[4].w;
            Far  = points[5].z / points[5].w;
        }

    public:
        /// <summary>
        /// Get planes for frustum
        /// </summary>
        [[nodiscard]] std::array<Math::Plane, 6> GetPlanes() const;

    public:
        /// <summary>
        /// Transform the frustum
        /// </summary>
        void Transform(
            const Math::TransformMatrix& transform)
        {
            Transform(transform.GetScale(), transform.GetRotation(), transform.GetPosition());
        }

        /// <summary>
        /// Transform the frustum
        /// </summary>
        void Transform(
            const Math::Vector3&    scale,
            const Math::Quaternion& rotation,
            const Math::Vector3&    translation);

    public:
        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const AABB& box) const;

        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const Frustum& fr) const;

        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            std::span<const Math::Plane> planes) const;
    };
} // namespace Ame::Geometry