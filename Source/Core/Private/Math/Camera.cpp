#include <Math/Camera.hpp>
#include <Math/Transform.hpp>

namespace Ame::Math
{
    Camera::Camera(
        CameraType type) :
        m_Type(type)
    {
        if (type == CameraType::Orthographic)
        {
            m_Viewport.FieldOfView = 90.0f;
            m_Viewport.NearPlane   = -1.f;
            m_Viewport.FarPlane    = 1.f;
        }
    }

    //

    void Camera::SetViewport(
        const Viewport& viewport)
    {
        m_Viewport = viewport;
        m_ProjectionMatrixCache.reset();
    }

    void Camera::SetType(
        CameraType type)
    {
        m_Type = type;
        m_ProjectionMatrixCache.reset();
    }

    auto Camera::GetViewport() const -> const Viewport&
    {
        return m_Viewport;
    }

    CameraType Camera::GetType() const
    {
        return m_Type;
    }

    //

    float Camera::Viewport::AspectRatio() const
    {
        return Width / Height;
    }

    Matrix4x4 Camera::Viewport::ProjectionMatrix(
        CameraType type) const
    {
        switch (type)
        {
        case CameraType::Perspective:
        {
            return Matrix4x4::PerspectiveFov(
                Util::DegToRad(FieldOfView),
                AspectRatio(),
                NearPlane,
                FarPlane);
        }
        case CameraType::Orthographic:
        {
            float xAxisMultiplier = 1.f;
            float yAxisMultiplier = 1.f / AspectRatio();

            if (!MaintainXFov)
            {
                std::swap(xAxisMultiplier, yAxisMultiplier);
            }

            float halfSize = OrthographicSize / 2.f;
            return Matrix4x4::Orthographic(
                Width * xAxisMultiplier * halfSize,
                Height * yAxisMultiplier * halfSize,
                NearPlane,
                FarPlane);
        }
        default:
            std::unreachable();
        }
    }

    const Matrix4x4& Camera::GetProjectionMatrix() const
    {
        if (!m_ProjectionMatrixCache)
        {
            m_ProjectionMatrixCache = m_Viewport.ProjectionMatrix(m_Type);
        }
        return *m_ProjectionMatrixCache;
    }

    Vector2 Camera::GetViewporSize() const
    {
        return { m_Viewport.Width, m_Viewport.Height };
    }
} // namespace Ame::Math