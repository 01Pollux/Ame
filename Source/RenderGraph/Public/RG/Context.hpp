#pragma once

#include <RG/ResourceStorage.hpp>
#include <RG/DependencyLevel.hpp>

namespace Ame::RG
{
    class Context
    {
        friend class PassStorage;
        friend class Graph;

        using DependencyLevelListType = std::vector<DependencyLevel>;

    public:
        /// <summary>
        /// Get the render graph storage
        /// </summary>
        [[nodiscard]] const ResourceStorage& GetStorage() const;

        /// <summary>
        /// Get the render graph storage
        /// </summary>
        [[nodiscard]] ResourceStorage& GetStorage();

    private:
        /// <summary>
        /// Execute the render graph with the specified execution context
        /// </summary>
        void Execute(
            Dg::IRenderDevice*             renderDevice,
            std::span<Dg::IDeviceContext*> contexts);

        /// <summary>
        /// Build the render graph
        /// </summary>
        void Build(
            DependencyLevelListType&& levels);

    private:
        ResourceStorage              m_Resources;
        std::vector<DependencyLevel> m_Levels;
    };
} // namespace Ame::RG