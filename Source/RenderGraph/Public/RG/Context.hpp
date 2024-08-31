#pragma once

#include <Rg/ResourceStorage.hpp>
#include <Rg/DependencyLevel.hpp>

namespace Ame::Rg
{
    class Context
    {
        friend class PassStorage;
        friend class Graph;

        using DependencyLevelListType = std::vector<DependencyLevel>;

    public:
        Context(Rhi::IRhiDevice* rhiDevice) : m_Resources(rhiDevice)
        {
        }

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
        void Execute(Dg::IDeviceContext* context);

        /// <summary>
        /// Build the render graph
        /// </summary>
        void Build(DependencyLevelListType&& levels);

    private:
        ResourceStorage              m_Resources;
        std::vector<DependencyLevel> m_Levels;
    };
} // namespace Ame::Rg