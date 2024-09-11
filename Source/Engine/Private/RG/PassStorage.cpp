#include <boost/range.hpp>
#include <boost/range/join.hpp>

#include <Rg/PassStorage.hpp>
#include <Rg/Context.hpp>
#include <Rg/DependencyLevel.hpp>

#include <Log/Logger.hpp>

namespace Ame::Rg
{
    PassStorage::~PassStorage() = default;

    Pass* PassStorage::AddPass(const String& name, UniquePtr<Pass> pass)
    {
        AME_LOG_ASSERT(!m_NamedPasses.contains(name), std::format("Pass with name '{}' already exists", name));

        auto iter = m_NamedPasses.emplace(name, std::move(pass)).first;
        m_Passes.emplace_back(iter);

        SetRebuildState(true);
        return iter->second.get();
    }

    void PassStorage::RemovePass(const String& name)
    {
        auto iter = m_NamedPasses.find(name);
        if (iter != m_NamedPasses.end())
        {
            std::erase(m_Passes, iter);
            m_NamedPasses.erase(iter);

            SetRebuildState(true);
        }
    }

    Pass* PassStorage::GetPass(const String& name) const
    {
        auto it = m_NamedPasses.find(name);
        return it != m_NamedPasses.end() ? it->second.get() : nullptr;
    }

    void PassStorage::Clear()
    {
        m_NamedPasses.clear();
        SetRebuildState(true);
    }

    //

    void PassStorage::Build(Context& context)
    {
        SetRebuildState(false);

        DependencyLevelListType levels;
        if (!m_Passes.empty())
        {
            auto resolvers = ResolvePasses(context);
            levels         = BuildPasses(resolvers);
        }
        context.Build(std::move(levels));
    }

    bool PassStorage::NeedsRebuild() const noexcept
    {
        return m_NeedsRebuild;
    }

    void PassStorage::SetRebuildState(bool state) noexcept
    {
        m_NeedsRebuild = state;
    }

    //

    auto PassStorage::ResolvePasses(Context& context) -> ResolverListType
    {
        ResolverListType resolvers;
        resolvers.reserve(m_Passes.size());

        return m_Passes |
               std::views::transform(
                   [&](auto& pass)
                   {
                       AME_LOG_TRACE(std::format("Building pass '{}'", pass->first));
                       Resolver resolver(context.GetStorage());
                       pass->second->DoBuild(resolver);
                       return resolver;
                   }) |
               std::ranges::to<std::vector>();
    }

    auto PassStorage::BuildPasses(ResolverListType& resolvers) -> DependencyLevelListType
    {
        auto adjacencyList         = BuildAdjacencyLists(resolvers);
        auto topologicalSortedList = TopologicalSort(adjacencyList);
        return BuildDependencyLevels(topologicalSortedList, adjacencyList);
    }

    //

    auto PassStorage::BuildAdjacencyLists(const ResolverListType& resolvers) -> AdjacencyListType
    {
        AdjacencyListType adjacencyList(m_Passes.size());

        for (size_t i = 0; i < m_Passes.size() - 1; i++)
        {
            auto& adjacencies = adjacencyList[i];
            auto& resolver    = resolvers[i];

            for (size_t j = i + 1; j < m_Passes.size(); j++)
            {
                auto& otherResolver = resolvers[j];
                for (auto& resource :
                     boost::range::join(otherResolver.m_ResourcesRead, otherResolver.m_ResourcesWritten))
                {
                    if (resolver.m_ResourcesWritten.contains(resource))
                    {
                        adjacencies.push_back(j);
                        break;
                    }
                }
            }
        }

        return adjacencyList;
    }

    //

    auto PassStorage::TopologicalSort(const AdjacencyListType& adjacencyList) -> TopologicalSortListType
    {
        std::stack<size_t> dfsStack{};
        std::vector<bool>  visitedList(m_Passes.size(), false);

        for (size_t i = 0; i < m_Passes.size(); i++)
        {
            if (!visitedList[i])
            {
                DepthFirstSearch(adjacencyList, i, visitedList, dfsStack);
            }
        }

        TopologicalSortListType topologicallySortedList;
        topologicallySortedList.reserve(dfsStack.size());

        while (!dfsStack.empty())
        {
            topologicallySortedList.push_back(dfsStack.top());
            dfsStack.pop();
        }

        return topologicallySortedList;
    }

    //

    void PassStorage::DepthFirstSearch(const AdjacencyListType& adjacencyList, size_t index,
                                       std::vector<bool>& visitedList, std::stack<size_t>& dfsStack)
    {
        visitedList[index] = true;
        for (size_t adjIndex : adjacencyList[index])
        {
            if (!visitedList[adjIndex])
            {
                DepthFirstSearch(adjacencyList, adjIndex, visitedList, dfsStack);
            }
        }
        dfsStack.push(index);
    }

    //

    auto PassStorage::BuildDependencyLevels(const TopologicalSortListType& topologicallySortedList,
                                            const AdjacencyListType&       adjacencyList) -> DependencyLevelListType
    {
        std::vector<size_t> distances(topologicallySortedList.size());
        for (size_t d = 0; d < distances.size(); d++)
        {
            size_t index = topologicallySortedList[d];
            for (size_t adjIndex : adjacencyList[index])
            {
                if (distances[adjIndex] < (distances[index] + 1))
                {
                    distances[adjIndex] = distances[index] + 1;
                }
            }
        }

        size_t size = std::ranges::max(distances) + 1;

        DependencyLevelListType Dependencies(size);
        for (size_t i = 0; i < m_Passes.size(); ++i)
        {
            size_t level = distances[i];
            Dependencies[level].AddPass(m_Passes[i]->second.get());
        }

        return Dependencies;
    }
} // namespace Ame::Rg