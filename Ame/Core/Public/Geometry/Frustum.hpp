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
        Math::Quaternion Orientation = Math::Vec::Identity<Math::Quaternion>;

        /// <summary>
        /// Origin of the frustum (and projection).
        /// </summary>
        Math::Vector3 Origin = Math::Vec::Zero<Math::Vector3>;

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
            const Math::Matrix4x4& InvProjection)
        {
            // Corners of the projection frustum in homogenous space.
            constexpr Math::Vector4 HomogenousPoints[6]{
                { 1.0f, 0.0f, 1.0f, 1.0f },  // right (at far plane)
                { -1.0f, 0.0f, 1.0f, 1.0f }, // left
                { 0.0f, 1.0f, 1.0f, 1.0f },  // top
                { 0.0f, -1.0f, 1.0f, 1.0f }, // bottom
                { 0.0f, 0.0f, 0.0f, 1.0f },  // near
                { 0.0f, 0.0f, 1.0f, 1.0f }   // far
            };

            // Compute the frustum corners in world space.
            Math::Vector4 Points[6]{
                InvProjection * HomogenousPoints[0],
                InvProjection * HomogenousPoints[1],
                InvProjection * HomogenousPoints[2],
                InvProjection * HomogenousPoints[3],
                InvProjection * HomogenousPoints[4],
                InvProjection * HomogenousPoints[5]
            };

            // Compute the slopes.
            RightSlope  = Points[0].x / Points[0].z;
            LeftSlope   = Points[1].x / Points[1].z;
            TopSlope    = Points[2].y / Points[2].z;
            BottomSlope = Points[3].y / Points[3].z;

            // Compute near and far.
            Near = Points[4].z / Points[4].w;
            Far  = Points[5].z / Points[5].w;
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
            const Math::TransformMatrix& Tr)
        {
            Transform(Tr.GetScale(), Tr.GetRotation(), Tr.GetPosition());
        }

        /// <summary>
        /// Transform the frustum
        /// </summary>
        void Transform(
            const Math::Vector3&    Scale,
            const Math::Quaternion& Rotation,
            const Math::Vector3&    Translation);

    public:
        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const AABB& Box) const;

        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const Frustum& Fr) const;

        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            std::span<const Math::Plane> Planes) const;
    };
} // namespace Ame::Geometry