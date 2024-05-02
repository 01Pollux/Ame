#include <Ecs/Component/Viewport/Camera.hpp>

#include <Math/Transform.hpp>

namespace Ame::Ecs::Component
{
    Camera::Camera(
        CameraType Type) :
        Type(Type)
    {
        if (Type == CameraType::Orthographic)
        {
            Viewport.FieldOfView = 90.0f;
            Viewport.NearPlane   = -1.f;
            Viewport.FarPlane    = 1.f;
        }
    }

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

    Math::Matrix4x4 Camera::GetProjectionMatrix() const
    {
        return Viewport.ProjectionMatrix(Type);
    }

    Math::Vector2 Camera::GetViewporSize() const
    {
        return { Viewport.Width, Viewport.Height };
    }
} // namespace Ame::Ecs::Component