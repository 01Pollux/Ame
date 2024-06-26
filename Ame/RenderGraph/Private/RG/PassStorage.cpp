#include <RG/PassStorage.hpp>
#include <RG/Context.hpp>
#include <RG/DependencyLevel.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    Pass* PassStorage::AddPass(
        const String& name,
        UPtr<Pass>    pass)
    {
        AME_LOG_ASSERT(Log::Gfx(), !m_NamedPasses.contains(name), "Pass with name '{}' already exists", name);
        auto iter = m_NamedPasses.emplace(name, std::move(pass)).first;
        m_Passes.emplace_back(iter);
        return iter->second.get();
    }

    void PassStorage::RemovePass(
        const String& name)
    {
        auto iter = m_NamedPasses.find(name);
        if (iter != m_NamedPasses.end())
        {
            std::erase(m_Passes, iter);
            m_NamedPasses.erase(iter);
        }
    }

    Pass* PassStorage::GetPass(
        const String& name) const
    {
        auto it = m_NamedPasses.find(name);
        return it != m_NamedPasses.end() ? it->second.get() : nullptr;
    }

    void PassStorage::Clear()
    {
        m_NamedPasses.clear();
    }

    //

    void PassStorage::Build(
        Context& context)
    {
        if (m_Passes.empty()) [[unlikely]]
        {
            context.Build({});
            return;
        }

        BuildersListType builders;
        builders.reserve(m_Passes.size());

        for (auto& pass : m_Passes)
        {
            auto& passStorage = builders.emplace_back(context.GetStorage());
            pass->second->DoBuild(passStorage.GraphResolver);
        }

        auto passes = BuildPasses(context, builders);
        context.Build(std::move(passes));
    }

    void PassStorage::RemoveOneShotPasses()
    {
        using namespace EnumBitOperators;

        std::erase_if(m_Passes, [](const PassIterator& pass)
                      { return (pass->second->GetFlags() & PassFlags::OneShot) == PassFlags::OneShot; });
    }

    //

    auto PassStorage::BuildPasses(
        Context&          context,
        BuildersListType& builders) -> DepepndencyLevelListType
    {
        auto adjacencyList         = BuildAdjacencyLists(builders);
        auto topologicalSortedList = TopologicalSort(adjacencyList);
        return BuildDependencyLevels(context, topologicalSortedList, adjacencyList, builders);
    }

    //

    auto PassStorage::BuildAdjacencyLists(
        const BuildersListType& builders) -> AdjacencyListType
    {
        AdjacencyListType adjacencyList(m_Passes.size());

        for (size_t i = 0; i < m_Passes.size() - 1; i++)
        {
            auto& adjacencies = adjacencyList[i];
            auto& resolver    = builders[i].GraphResolver;

            for (size_t j = i + 1; j < m_Passes.size(); j++)
            {
                auto& otherResolver = builders[j].GraphResolver;
                for (auto& otherPassRead : otherResolver.m_ResourcesRead)
                {
                    if (resolver.m_ResourcesWritten.contains(otherPassRead))
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

    auto PassStorage::TopologicalSort(
        const AdjacencyListType& adjacencyList) -> TopologicalSortListType
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

    void PassStorage::DepthFirstSearch(
        const AdjacencyListType& adjacencyList,
        size_t                   index,
        std::vector<bool>&       visitedList,
        std::stack<size_t>&      dfsStack)
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

    auto PassStorage::BuildDependencyLevels(
        Context&                       context,
        const TopologicalSortListType& topologicallySortedList,
        const AdjacencyListType&       adjacencyList,
        BuildersListType&              builders) -> DepepndencyLevelListType
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

        DepepndencyLevelListType Dependencies(size);
        for (size_t i = 0; i < m_Passes.size(); ++i)
        {
            size_t level    = distances[i];
            auto&  builder  = builders[i];
            auto&  resolver = builders[i].GraphResolver;

            Dependencies[level].AddPass(
                context,
                m_Passes[i]->second.get(),
                std::move(resolver.m_RenderTargets),
                std::move(resolver.m_DepthStencil),
                std::move(resolver.m_ResourcesCreated),
                resolver.m_ResourceStates,
                resolver.m_TextureLayouts);
        }

        return Dependencies;
    }
} // namespace Ame::RG