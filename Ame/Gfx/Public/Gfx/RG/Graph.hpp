#pragma once

#include <Gfx/RG/PassStorage.hpp>
#include <Gfx/RG/Context.hpp>

#include <Frame/FrameTimer.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx::RG
{
    class Graph
    {
    public:
        Graph(
            FrameTimer&    frameTimer,
            Rhi::Device&   rhiDevice,
            Ecs::Universe& universe);

    public:
        /// <summary>
        /// Build the render graph if needed
        /// </summary>
        void Update();

        /// <summary>
        /// Update the frame storage
        /// </summary>
        void UpdateFrameStorage(
            const Ecs::Entity&           cameraEntity,
            const Math::TransformMatrix& transform,
            const Math::Matrix4x4&       projection,
            const Math::Vector2&         viewport);

        /// <summary>
        /// Build and execute the render graph
        /// </summary>
        void Execute();

    public:
        /// <summary>
        /// Get the render graph builder
        /// </summary>
        [[nodiscard]] PassStorage& GetPassStorage();

        /// <summary>
        /// Get the render graph storage
        /// </summary>
        [[nodiscard]] const ResourceStorage& GetResourceStorage() const;

        /// <summary>
        /// Get the render graph storage
        /// </summary>
        [[nodiscard]] ResourceStorage& GetResourceStorage();

    private:
        Ref<FrameTimer>  m_Timer;
        Ref<Rhi::Device> m_Device;

        Context     m_Context;
        PassStorage m_Passes;
    };
} // namespace Ame::Gfx::RG