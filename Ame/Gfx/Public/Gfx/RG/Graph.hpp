#pragma once

#include <Gfx/RG/PassStorage.hpp>
#include <Gfx/RG/Context.hpp>

#include <Frame/Timer.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx::RG
{
    class Graph
    {
    public:
        Graph(
            FrameTimer&  Timer,
            Rhi::Device& Device);

    public:
        /// <summary>
        /// Build the render graph if needed
        /// </summary>
        void Update();

        /// <summary>
        /// Update the frame storage
        /// </summary>
        void UpdateFrameStorage(
            const Math::TransformMatrix& Transform,
            const Math::Matrix4x4&       Projection,
            const Math::Vector2&         Viewport);

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
        PassStorage m_Builder;
    };
} // namespace Ame::Gfx::RG