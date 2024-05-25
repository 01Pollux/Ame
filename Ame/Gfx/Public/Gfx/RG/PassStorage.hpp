#pragma once

#include <map>
#include <stack>
#include <Gfx/RG/Pass.hpp>

namespace Ame::Gfx::RG
{
    class PassStorage
    {
        friend class Graph;

        struct BuilderInfo
        {
            Resolver GraphResolver;
            BuilderInfo(
                ResourceStorage& storage) :
                GraphResolver(storage)
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
            UPtr<Pass> pass);

        /// <summary>
        /// Add a render pass to the graph
        /// </summary>
        template<typename Ty, typename... ArgsTy>
            requires std::derived_from<Ty, Pass>
        Ty& NewPass(
            ArgsTy&&... args)
        {
            auto  pass    = std::make_unique<Ty>(std::forward<ArgsTy>(args)...);
            auto& passRef = *pass;
            AddPass(std::move(pass));
            return passRef;
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
            const Pass* pass);

        /// <summary>
        /// Clear all render passes from the graph
        /// </summary>
        void Clear();

        /// <summary>
        /// Check if the graph contains a render pass with the given name
        /// </summary>
        [[nodiscard]] bool ContainsPass(
            const Pass* pass);

    private:
        /// <summary>
        /// Build render graph from graph builder
        /// </summary>
        void Build(
            Context& context);

        /// <summary>
        /// Remove one shot passes from the graph
        /// </summary>
        void RemoveOneShotPasses();

    private:
        /// <summary>
        /// Build passes from builders
        /// </summary>
        [[nodiscard]] DepepndencyLevelListType BuildPasses(
            Context&          context,
            BuildersListType& builders);

        /// <summary>
        /// Build adjacency lists for passes dependencies
        /// </summary>
        AdjacencyListType BuildAdjacencyLists(
            const BuildersListType& builders);

        /// <summary>
        /// Topological sort of the graph
        /// </summary>
        TopologicalSortListType TopologicalSort(
            const AdjacencyListType& adjacencyList);

        /// <summary>
        /// Depth first search for topological sort
        /// </summary>
        void DepthFirstSearch(
            const AdjacencyListType& adjacencyList,
            size_t                   index,
            std::vector<bool>&       visitedList,
            std::stack<size_t>&      dfsStack);

        /// <summary>
        /// Build dependency levels
        /// </summary>
        [[nodiscard]] DepepndencyLevelListType BuildDependencyLevels(
            Context&                       context,
            const TopologicalSortListType& topologicallySortedList,
            const AdjacencyListType&       adjacencyList,
            BuildersListType&              builders);

    private:
        std::vector<UPtr<Pass>> m_Passes;
    };
} // namespace Ame::Gfx::RG