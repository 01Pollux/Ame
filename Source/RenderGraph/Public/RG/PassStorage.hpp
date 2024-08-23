#pragma once

#include <map>
#include <stack>
#include <Rg/Pass.hpp>

namespace Ame::Rg
{
    class PassStorage
    {
        friend class Graph;

        using ResolverListType        = std::vector<Resolver>;
        using TopologicalSortListType = std::vector<size_t>;
        using AdjacencyListType       = std::vector<TopologicalSortListType>;
        using DependencyLevelListType = std::vector<DependencyLevel>;

        static constexpr std::chrono::milliseconds s_GraphBuildTime = std::chrono::milliseconds(5'000);

    public:
        /// <summary>
        /// Add a render pass to the graph
        /// </summary>
        Pass* AddPass(
            const String&   name,
            UniquePtr<Pass> pass);

        /// <summary>
        /// Add a render pass to the graph
        /// </summary>
        template<typename Ty, typename... ArgsTy>
            requires std::derived_from<Ty, Pass>
        Ty& NewPass(
            const String& name,
            ArgsTy&&... args)
        {
            auto  pass    = std::make_unique<Ty>(std::forward<ArgsTy>(args)...);
            auto& passRef = *pass;
            AddPass(name, std::move(pass));
            return passRef;
        }

        /// <summary>
        /// Add a render pass to the graph
        /// </summary>
        template<typename Ty = void>
        [[nodiscard]] TypedPass<Ty>& NewTypedPass(
            const String& name)
        {
            return NewPass<TypedPass<Ty>>(name);
        }

        /// <summary>
        /// Remove a render pass from the graph
        /// </summary>
        void RemovePass(
            const String& name);

        /// <summary>
        /// Get a render pass from the graph or nullptr if not found
        /// </summary>
        [[nodiscard]] Pass* GetPass(
            const String& name) const;

        /// <summary>
        /// Clear all render passes from the graph
        /// </summary>
        void Clear();

    private:
        /// <summary>
        /// Build render graph from graph builder
        /// </summary>
        void Build(
            Context& context);

        /// <summary>
        /// Check if passes were changed and needs to be rebuilt
        /// </summary>
        [[nodiscard]] bool NeedsRebuild() const noexcept;

        /// <summary>
        /// Set rebuild state
        /// </summary>
        void SetRebuildState(
            bool state) noexcept;

    private:
        [[nodiscard]] ResolverListType        ResolvePasses(Context& context);
        [[nodiscard]] DependencyLevelListType BuildPasses(ResolverListType& resolvers);

    private:
        AdjacencyListType                     BuildAdjacencyLists(const ResolverListType& resolvers);
        TopologicalSortListType               TopologicalSort(const AdjacencyListType& adjacencyList);
        void                                  DepthFirstSearch(const AdjacencyListType& adjacencyList, size_t index, std::vector<bool>& visitedList, std::stack<size_t>& dfsStack);
        [[nodiscard]] DependencyLevelListType BuildDependencyLevels(const TopologicalSortListType& topologicallySortedList, const AdjacencyListType& adjacencyList);

    private:
        using PassMap      = std::map<String, UniquePtr<Pass>>;
        using PassIterator = PassMap::const_iterator;
        using PassList     = std::vector<PassIterator>;

        PassMap  m_NamedPasses;
        PassList m_Passes;

        bool m_NeedsRebuild : 1 = false;
    };
} // namespace Ame::Rg