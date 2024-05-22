#include <Ecs/Component/Viewport/Camera.hpp>

#include <Math/Transform.hpp>

namespace Ame::Ecs::Component
{
    Camera::Camera(
        CameraType Type) :
        m_Type(Type)
    {
        if (Type == CameraType::Orthographic)
        {
            m_Viewport.FieldOfView = 90.0f;
            m_Viewport.NearPlane   = -1.f;
            m_Viewport.FarPlane    = 1.f;
        }
    }

    //

    void Camera::SetViewport(
        const Viewport& Viewport)
    {
        m_Viewport = Viewport;
		m_ProjectionMatrixCache.reset();
    }

    void Camera::SetType(
        CameraType Type)
    {
        m_Type = Type;
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

    Math::Matrix4x4 Camera::Viewport::ProjectionMatrix(
        CameraType Type) const
    {
        switch (Type)
        {
        case CameraType::Perspective:
        {
            return glm::perspectiveFov(
                FieldOfView,
                Width,
                Height,
                NearPlane,
                FarPlane);
        }
        case CameraType::Orthographic:
        {
            float XAxisMultiplier = 1.f;
            float YAxisMultiplier = 1.f / AspectRatio();

            if (!MaintainXFov)
            {
                std::swap(XAxisMultiplier, YAxisMultiplier);
            }

            float HalfSize = OrthographicSize / 2.f;
            return glm::ortho(
                -HalfSize * XAxisMultiplier,
                HalfSize * XAxisMultiplier,
                -HalfSize * YAxisMultiplier,
                HalfSize * YAxisMultiplier,
                NearPlane,
                FarPlane);
        }
        default:
            return Math::Mat::Identity<Math::Matrix4x4>;
        }
    }

    const Math::Matrix4x4& Camera::GetProjectionMatrix() const
    {
        if (!m_ProjectionMatrixCache)
        {
            m_ProjectionMatrixCache = m_Viewport.ProjectionMatrix(m_Type);
        }
        return *m_ProjectionMatrixCache;
    }

    Math::Vector2 Camera::GetViewporSize() const
    {
        return { m_Viewport.Width, m_Viewport.Height };
    }
} // namespace Ame::Ecs::Component