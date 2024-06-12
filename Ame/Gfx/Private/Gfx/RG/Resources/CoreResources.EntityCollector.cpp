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
        if (!CollectEntitiesInOctree(renderRule))
        {
            CollectEntitiesInFrustum(renderRule);
        }
        m_CameraCullResult.Upload();
    }

    //

    bool CoreResources::CollectEntitiesInOctree(
        const CameraRenderRule& renderRule)
    {
        return false;
    }

    void CoreResources::CollectEntitiesInFrustum(
        const CameraRenderRule& renderRule)
    {
        auto&               cameraData = m_FrameResource.CurrentCamera.GetComponent<Ecs::Component::Camera>();
        const Math::Vector3 cameraPosition{
            m_FrameResource.World(0, 3),
            m_FrameResource.World(1, 3),
            m_FrameResource.World(2, 3)
        };

        auto filterEntities =
            [this, &cameraData, &cameraPosition](
                Ecs::Iterator&                        iter,
                const Ecs::Component::Transform*      transforms,
                const TransformBuffer::GpuId*         transformIds,
                const Ecs::Component::BaseRenderable* renderables)
        {
            for (auto i : iter)
            {
                auto  ent         = iter.entity(i).name();
                auto& transform   = transforms[i];
                auto  transformId = transformIds[i];
                auto& renderable  = renderables[i];

                if ((renderable.CameraMask & cameraData.CullMask) != cameraData.CullMask)
                {
                    continue;
                }

                float distSqr = transform.GetPosition().DistanceToSqr(cameraPosition);
                m_CameraCullResult.AddEntity(distSqr, renderable, transformId);
            }
        };

        renderRule->iter(filterEntities);
    }
} // namespace Ame::Gfx::RG