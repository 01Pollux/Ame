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
            float FarPlane = 10000.0f;

            /// <summary>
            /// The field of view of the viewport.
            /// </summary>
            float FieldOfView = 45.0f;

            /// <summary>
            /// The width and height of the viewport.
            /// </summary>
            float Width = 1024.f;

            /// <summary>
            /// The width and height of the viewport.
            /// </summary>
            float Height = 720.f;

            /// <summary>
            /// The orthographic size of the viewport.
            /// </summary>
            float OrthographicSize = 1.0f;

            /// <summary>
            /// Maintain the x field of view when resizing.
            /// </summary>
            bool MaintainXFov : 1 = true;

            /// <summary>
            /// Automatically calculate the width based on client size.
            /// </summary>
            float ClientWidth = 1.f;

            /// <summary>
            /// Automatically calculate the height based on client size.
            /// </summary>
            float ClientHeight = 1.f;

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
            CameraType Type = CameraType::Perspective) noexcept;

    public:
        void SetViewport(
            const Viewport& viewport) noexcept;

        void SetType(
            CameraType type) noexcept;

        [[nodiscard]] const Viewport& GetViewport() const noexcept;

        /// <summary>
        /// Get viewport without marking it changed, if the viewport has changed, call MarkChanged.
        /// </summary>
        [[nodiscard]] Viewport& GetViewport() noexcept;

        [[nodiscard]] CameraType GetType() const noexcept;

        void MarkChanged() noexcept;

    public:
        /// <summary>
        /// Get the projection matrix of the viewport.
        /// </summary>
        [[nodiscard]] const Math::Matrix4x4& GetProjectionMatrix() const;

        /// <summary>
        /// Get viewport size.
        /// </summary>
        [[nodiscard]] Math::Vector2 GetViewporSize() const noexcept;

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
} // namespace Ame::Math