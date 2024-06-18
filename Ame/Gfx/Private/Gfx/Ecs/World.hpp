#pragma once

#include <Ecs/Signals/Universe.hpp>
#include <Ecs/Universe.hpp>

#include <Gfx/RenderGraph/Buffers/TransformBuffer.hpp>
#include <Gfx/RenderGraph/Buffers/AABBBuffer.hpp>
#include <Gfx/RenderGraph/Buffers/InstanceBuffer.hpp>

namespace Ame::Ecs::Component
{
    struct Transform;
    struct BaseRenderable;
} // namespace Ame::Ecs::Component

namespace Ame::Gfx
{
    struct EcsWorldResourcesDesc;

    class EcsWorldResources
    {
    public:
        using CameraRenderRule = Ecs::UniqueRule<
            const RenderInstance::GpuId,
            const Ecs::Component::Transform,
            const Ecs::Component::BaseRenderable>;

    public:
        EcsWorldResources(
            Rhi::Device&                 rhiDevice,
            Ecs::Universe&               universe,
            const EcsWorldResourcesDesc& desc);

    public:
        /// <summary>
        /// Returns the transform buffer.
        /// </summary>
        [[nodiscard]] TransformBuffer& GetTransformBuffer() noexcept
        {
            return m_TransformBuffer;
        }

        /// <summary>
        /// Returns the AABB buffer.
        /// </summary>
        [[nodiscard]] AABBBuffer& GetAABBBuffer() noexcept
        {
            return m_AABBBuffer;
        }

        /// <summary>
        /// Returns the AABB buffer.
        /// </summary>
        [[nodiscard]] InstanceBuffer& GetInstanceBuffer() noexcept
        {
            return m_InstanceBuffer;
        }

    public:
        /// <summary>
        /// Returns the camera render rule.
        /// </summary>
        [[nodiscard]] CameraRenderRule& GetCameraRenderRule() noexcept
        {
            return m_WorldData.RenderRule;
        }

    private:
        void RegisterModules(
            Ecs::World& world);

    private:
        void CreateObserversAndRules(
            Ecs::World& world);

    private:
        struct EntityDesc
        {
            CameraRenderRule    RenderRule;
            Ecs::UniqueObserver TransformObserver;
            Ecs::UniqueObserver AABBObserver;
            Ecs::UniqueObserver InstanceObserver;
        };

    private:
        Ref<Ecs::Universe> m_Universe;

        Signals::ScopedConnection m_OnWorldChange;
        EntityDesc                m_WorldData;

        TransformBuffer m_TransformBuffer;
        AABBBuffer      m_AABBBuffer;
        InstanceBuffer  m_InstanceBuffer;
    };
} // namespace Ame::Gfx