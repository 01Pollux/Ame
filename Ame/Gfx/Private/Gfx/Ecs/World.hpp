#pragma once

#include <Ecs/Signals/Universe.hpp>
#include <Ecs/Universe.hpp>

#include <Gfx/RenderGraph/Buffers/TransformBuffer.hpp>
#include <Gfx/RenderGraph/Buffers/AABBBuffer.hpp>

namespace Ame::Gfx
{
    struct EcsWorldResourcesDesc;

    class EcsWorldResources
    {
        struct EntityDesc
        {
            Ecs::UniqueObserver TransformObserver;
            Ecs::UniqueObserver AABBObserver;
        };

    public:
        EcsWorldResources(
            Rhi::Device&                 rhiDevice,
            Ecs::Universe&               universe,
            const EcsWorldResourcesDesc& desc);

    public:
        /// <summary>
        /// Returns the transform buffer.
        /// </summary>
        [[nodiscard]] TransformBuffer& GetTransformBuffer()
        {
            return m_TransformBuffer;
        }

        /// <summary>
        /// Returns the AABB buffer.
        /// </summary>
        [[nodiscard]] AABBBuffer& GetAABBBuffer()
        {
            return m_AABBBuffer;
        }

    private:
        void RegisterModules(
            Ecs::World& world);

    private:
        void CreateObservers(
            Ecs::World& world);

    private:
        Ref<Ecs::Universe> m_Universe;

        Signals::ScopedConnection m_OnWorldChange;
        EntityDesc                m_WorldData;

        TransformBuffer m_TransformBuffer;
        AABBBuffer      m_AABBBuffer;
    };
} // namespace Ame::Gfx