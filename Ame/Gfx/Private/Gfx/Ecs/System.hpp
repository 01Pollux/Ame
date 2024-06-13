#pragma once

#include <Ecs/Signals/Universe.hpp>
#include <Ecs/Universe.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>

#include <Gfx/RenderGraph/Buffers/TransformBuffer.hpp>
#include <Gfx/RenderGraph/Buffers/AABBBuffer.hpp>

namespace Ame::Gfx
{
    struct EcsSystemDesc
    {
        TransformBuffer::DescType TransformBufferDesc;
        AABBBuffer::DescType      AABBBufferDesc;
    };

    using CameraRenderRule = Ecs::UniqueRule<
        const Ecs::Component::Transform,
        const TransformBuffer::GpuId,
        const Ecs::Component::BaseRenderable>;

    class EcsSystemHooks
    {
        struct EntityDesc
        {
            Ecs::UniqueObserver TransformObserver;
            CameraRenderRule    RenderRule;
        };

    public:
        EcsSystemHooks(
            Rhi::Device&         rhiDevice,
            Ecs::Universe&       universe,
            const EcsSystemDesc& desc = {});

    public:
        [[nodiscard]] CameraRenderRule& GetCameraRule() noexcept
        {
            return m_WorldData.RenderRule;
        }

    private:
        void RegisterModules(
            Ecs::World& world);

    private:
        void CreateTransformObserver(
            Ecs::World& world);
        void CreateCameraRule(
            Ecs::World& world);

    private:
        Ref<Ecs::Universe> m_Universe;

        Signals::ScopedConnection m_OnWorldChange;
        EntityDesc                m_WorldData;

        TransformBuffer m_TransformBuffer;
        AABBBuffer      m_AABBBuffer;
    };
} // namespace Ame::Gfx