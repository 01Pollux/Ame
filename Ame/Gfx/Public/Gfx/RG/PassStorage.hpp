#pragma once

#include <map>
#include <stack>
#include <Gfx/RG/Pass.hpp>
#include <Gfx/RG/Standard.hpp>

namespace Ame::Gfx::RG
{
    class PassStorage
    {
        friend class Graph;

        struct BuilderInfo
        {
            Resolver RgResolver;
            BuilderInfo(
                ResourceStorage& RgStorage) :
                RgResolver(RgStorage)
            {
            }
        };

        using BuildersListType         = std::vector<BuilderInfo>;
        using TopologicalSortListType  = std::vector<size_t>;
        using AdjacencyListType        = std::vector<TopologicalSortListType>;
        using DepepndencyLevelListType = std::vector<DependencyLevel>;

    public:
        /// <summary>
        /// Add a render pass to the graph
        /// </summary>
        Pass* AddPass(
            UPtr<Pass> Pass);

        /// <summary>
        /// Add a render pass to the graph
        /// </summary>
        template<typename Ty, typename... ArgsTy>
            requires std::derived_from<Ty, Pass>
        Ty& NewPass(
            ArgsTy&&... Args)
        {
            auto  PassPtr = std::make_unique<Ty>(std::forward<ArgsTy>(Args)...);
            auto& PassRef = *PassPtr;
            AddPass(std::move(PassPtr));
            return PassRef;
        }

        /// <summary>
        /// Add a render pass to the graph
        /// </summary>
        template<typename Ty = void>
        [[nodiscard]] TypedPass<Ty>& NewTypedPass()
        {
            return NewPass<TypedPass<Ty>>();
        }

        /// <summary>
        /// Remove a render pass from the graph
        /// </summary>
        UPtr<Pass> RemovePass(
            const Pass* RgPass);

        /// <summary>
        /// Clear all render passes from the graph
        /// </summary>
        void Clear();

        /// <summary>
        /// Check if the graph contains a render pass with the given name
        /// </summary>
        [[nodiscard]] bool ContainsPass(
            const Pass* RgPass);

    public:
        /// <summary>
        /// Set the graph as one of the standard graphs
        /// </summary>
        void SetStandard(
            StdGraph Type);

    private:
        /// <summary>
        /// Build render graph from graph builder
        /// </summary>
        void Build(
            Context& RgContext);

        /// <summary>
        /// Remove one shot passes from the graph
        /// </summary>
        void RemoveOneShotPasses();

    private:
        /// <summary>
        /// Build passes from builders
        /// </summary>
        [[nodiscard]] DepepndencyLevelListType BuildPasses(
            Context&          RgContext,
            BuildersListType& Builders);

        /// <summary>
        /// Build adjacency lists for passes dependencies
        /// </summary>
        AdjacencyListType BuildAdjacencyLists(
            const BuildersListType& Builders);

        /// <summary>
        /// Topological sort of the graph
        /// </summary>
        TopologicalSortListType TopologicalSort(
            const AdjacencyListType& AdjacencyList);

        /// <summary>
        /// Depth first search for topological sort
        /// </summary>
        void DepthFirstSearch(
            const AdjacencyListType& AdjacencyList,
            size_t                   Index,
            std::vector<bool>&       Visited,
            std::stack<size_t>&      Stack);

        /// <summary>
        /// Build dependency levels
        /// </summary>
        [[nodiscard]] DepepndencyLevelListType BuildDependencyLevels(
            Context&                       RgContext,
            const TopologicalSortListType& TopologicalSort,
            const AdjacencyListType&       AdjacencyList,
            BuildersListType&              Builders);

    private:
        std::vector<UPtr<Pass>> m_Passes;
    };
} // namespace Ame::Gfx::RG