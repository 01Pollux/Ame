#include <Gfx/RG/Resources/CoreResources.hpp>

#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <Ecs/Component/Math/Transform.hpp>

namespace Ame::Gfx::RG
{
    void CoreResources::ResetCameraStorage()
    {
        m_CameraCullResult.Reset();
    }

    void CoreResources::CollectEntities()
    {
        CollectEntities(m_EcsSystemHooks.GetCameraRule());
    }

    void CoreResources::CollectEntities(
        const CameraRenderRule& RenderRule)
    {
        auto World = m_Universe.get().GetActiveWorld();

        auto&               CameraData = m_FrameResource.CurrentCamera.GetComponent<Ecs::Component::Camera>();
        const Math::Vector3 CameraPosition{
            m_FrameResource.World[0][3],
            m_FrameResource.World[1][3],
            m_FrameResource.World[2][3]
        };

        auto FilterEntities =
            [this, &CameraData, &CameraPosition](
                Ecs::Iterator&                        Iter,
                RenderInstance*                       Instances,
                const Ecs::Component::Transform*      Transforms,
                const Ecs::Component::BaseRenderable* Renderables)
        {
            for (auto i : Iter)
            {
                auto& Instance   = Instances[i];
                auto& Transform  = Transforms[i];
                auto& Renderable = Renderables[i];

                if ((Renderable.CameraMask & CameraData.CullMask) != CameraData.CullMask)
                {
                    continue;
                }

                float Distance = glm::distance2(Transform.GetPosition(), CameraPosition);
                m_CameraCullResult.AddEntity(Distance, Renderable, Instance);
            }
        };

        RenderRule->iter(FilterEntities);
        m_CameraCullResult.Upload();
    }
} // namespace Ame::Gfx::RG