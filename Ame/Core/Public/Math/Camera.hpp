#pragma once

#include <Core/Ame.hpp>
#include <Math/Vector.hpp>
#include <Math/Matrix.hpp>

namespace Ame::Math
{
    enum class CameraType : uint8_t
    {
        Perspective,
        Orthographic
    };

    struct Camera
    {
        enum class Type : uint8_t
        {
            Perspective,
            Orthographic
        };

        struct Viewport
        {
            /// <summary>
            /// The near and far plane of the viewport.
            /// </summary>
            float NearPlane = 0.1f;

            /// <summary>
            /// The far plane of the viewport.
            /// </summary>
            float FarPlane = 1000.0f;

            /// <summary>
            /// The field of view of the viewport.
            /// </summary>
            float FieldOfView = 45.0f;

            /// <summary>
            /// The width and height of the viewport.
            /// </summary>
            float Width = 800.f;

            /// <summary>
            /// The width and height of the viewport.
            /// </summary>
            float Height = 600.f;

            /// <summary>
            /// The orthographic size of the viewport.
            /// </summary>
            float OrthographicSize = 1.0f;

            /// <summary>
            /// Maintain the x field of view when resizing.
            /// </summary>
            bool MaintainXFov : 1 = true;

            /// <summary>
            /// Get the aspect ratio of the viewport.
            /// </summary>
            [[nodiscard]] float AspectRatio() const;

            /// <summary>
            /// Get the projection matrix of the viewport.
            /// </summary>
            [[nodiscard]] Math::Matrix4x4 ProjectionMatrix(
                CameraType type) const;
        };

        Camera(
            CameraType Type = CameraType::Perspective);

    public:
        void SetViewport(
            const Viewport& viewport);

        void SetType(
            CameraType type);

        [[nodiscard]] const Viewport& GetViewport() const;

        [[nodiscard]] CameraType GetType() const;

        /// <summary>
        /// The culling mask of the camera.
        /// </summary>
        uint32_t ViewMask = 0xFFFFFFFF;

        /// <summary>
        /// The render order of the camera.
        /// </summary>
        int Priority = 0;

    public:
        /// <summary>
        /// Get the projection matrix of the viewport.
        /// </summary>
        [[nodiscard]] const Math::Matrix4x4& GetProjectionMatrix() const;

        /// <summary>
        /// Get viewport size.
        /// </summary>
        [[nodiscard]] Math::Vector2 GetViewporSize() const;

    private:
        /// <summary>
        /// The viewport of the camera.
        /// </summary>
        Viewport m_Viewport;

        mutable Opt<Math::Matrix4x4> m_ProjectionMatrixCache;

        /// <summary>
        /// The type of the camera.
        /// </summary>
        CameraType m_Type = CameraType::Perspective;
    };
} // namespace Ame::Ecs::Component