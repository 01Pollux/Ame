#pragma once

#include <RG/PassStorage.hpp>
#include <RG/Context.hpp>

namespace Ame::RG
{
    class Graph
    {
    public:
        /// <summary>
        /// Execute the render graph with the immediate context
        /// </summary>
        void Execute(
            Rhi::IRhiDevice* rhiDevice);

        /// <summary>
        /// Execute the render graph with the specified execution context
        /// </summary>
        void Execute(
            Rhi::IRhiDevice*               rhiDevice,
            std::span<Dg::IDeviceContext*> contexts);

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
        Context     m_Context;
        PassStorage m_Passes;
    };
} // namespace Ame::RG