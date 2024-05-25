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
        const CameraRenderRule& renderRule)
    {
        auto world = m_Universe.get().GetActiveWorld();

        auto&               cameraData = m_FrameResource.CurrentCamera.GetComponent<Ecs::Component::Camera>();
        const Math::Vector3 cameraPosition{
            m_FrameResource.World[0][3],
            m_FrameResource.World[1][3],
            m_FrameResource.World[2][3]
        };

        auto FilterEntities =
            [this, &cameraData, &cameraPosition](
                Ecs::Iterator&                        iter,
                RenderInstance*                       instances,
                const Ecs::Component::Transform*      transforms,
                const Ecs::Component::BaseRenderable* renderables)
        {
            for (auto i : iter)
            {
                auto  ent        = iter.entity(i).name();
                auto& Instance   = instances[i];
                auto& Transform  = transforms[i];
                auto& Renderable = renderables[i];

                if ((Renderable.CameraMask & cameraData.CullMask) != cameraData.CullMask)
                {
                    continue;
                }

                float Distance = glm::distance2(Transform.GetPosition(), cameraPosition);
                m_CameraCullResult.AddEntity(Distance, Renderable, Instance);
            }
        };

        renderRule->iter(FilterEntities);
        m_CameraCullResult.Upload();
    }
} // namespace Ame::Gfx::RG