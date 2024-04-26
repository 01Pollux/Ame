#include <Gfx/RG/Builder.hpp>
#include <Gfx/RG/Context.hpp>
#include <Gfx/RG/DependencyLevel.hpp>

namespace Ame::Gfx::RG
{
    Pass* Builder::AddPass(
        UPtr<Pass> RgPass)
    {
        return m_Passes.emplace_back(std::move(RgPass)).get();
    }

    UPtr<Pass> Builder::RemovePass(
        const Pass* RgPass)
    {
        UPtr<Pass> RemovedPass;
        std::erase_if(m_Passes,
                      [&](auto& CurPass)
                      {
                          if (CurPass.get() == RgPass)
                          {
                              RemovedPass = std::move(CurPass);
                              return true;
                          }
                          return false;
                      });
        return RemovedPass;
    }

    void Builder::Clear()
    {
        m_Passes.clear();
    }

    bool Builder::ContainsPass(
        const Pass* RgPass)
    {
        return std::ranges::contains(m_Passes, RgPass, [](auto& CurPass)
                                     { return CurPass.get(); });
    }

    //

    void Builder::Build(
        Context& RgContext)
    {
        if (m_IsBuilt)
        {
            return;
        }

        BuildersListType Builders;

        Builders.reserve(m_Passes.size());
        for (size_t i = 0; i < m_Passes.size(); i++)
        {
            auto& Builder = Builders.emplace_back(RgContext.GetStorage());
            m_Passes[i]->DoBuild(Builder.RgResolver);
        }
        RgContext.Build(BuildPasses(RgContext, Builders));
    }

    void Builder::RemoveOneShotPasses()
    {
        using namespace EnumBitOperators;

        bool ErasedPasses =
            std::erase_if(m_Passes, [](const UPtr<Pass>& RgPass)
                          { return (RgPass->GetFlags() & PassFlags::OneShot) == PassFlags::OneShot; }) > 0;

        m_IsBuilt = m_IsBuilt && !ErasedPasses;
    }

    //

    auto Builder::BuildPasses(
        Context&          RgContext,
        BuildersListType& Builders) -> DepepndencyLevelListType
    {
        auto AdjacencyList         = BuildAdjacencyLists(Builders);
        auto TopologicalSortedList = TopologicalSort(AdjacencyList);
        return BuildDependencyLevels(RgContext, TopologicalSortedList, AdjacencyList, Builders);
    }

    //

    auto Builder::BuildAdjacencyLists(
        const BuildersListType& Builders) -> AdjacencyListType
    {
        AdjacencyListType AdjacencyList(m_Passes.size());

        for (size_t i = 0; i < m_Passes.size(); i++)
        {
            auto& Adjacencies = AdjacencyList[i];
            auto& CurResolver = Builders[i].RgResolver;

            for (size_t j = i + 1; j < m_Passes.size(); j++)
            {
                auto& OtherResolver = Builders[j].RgResolver;
                bool  Depends       = false;

                for (auto& OtherPassRead : OtherResolver.m_ResourcesRead)
                {
                    if (CurResolver.m_ResourcesWritten.contains(OtherPassRead))
                    {
                        Adjacencies.push_back(j);
                        Depends = true;
                        break;
                    }
                }
            }
        }

        return AdjacencyList;
    }

    //

    auto Builder::TopologicalSort(
        const AdjacencyListType& AdjacencyList) -> TopologicalSortListType
    {
        std::stack<size_t> Stack{};
        std::vector<bool>  Visited(m_Passes.size(), false);
        for (size_t i = 0; i < m_Passes.size(); i++)
        {
            if (!Visited[i])
            {
                DepthFirstSearch(AdjacencyList, i, Visited, Stack);
            }
        }

        TopologicalSortListType TopologicallySortedList;
        TopologicallySortedList.reserve(Stack.size());
        while (!Stack.empty())
        {
            TopologicallySortedList.push_back(Stack.top());
            Stack.pop();
        }

        return TopologicallySortedList;
    }

    //

    void Builder::DepthFirstSearch(
        const AdjacencyListType& AdjacencyList,
        size_t                   Index,
        std::vector<bool>&       Visited,
        std::stack<size_t>&      Stack)
    {
        Visited[Index] = true;
        for (size_t AdjIndex : AdjacencyList[Index])
        {
            if (!Visited[AdjIndex])
            {
                DepthFirstSearch(AdjacencyList, AdjIndex, Visited, Stack);
            }
        }
        Stack.push(Index);
    }

    //

    auto Builder::BuildDependencyLevels(
        Context&                       RgContext,
        const TopologicalSortListType& TopologicalSort,
        const AdjacencyListType&       AdjacencyList,
        BuildersListType&              Builders) -> DepepndencyLevelListType
    {
        std::vector<size_t> Distances(TopologicalSort.size());
        for (size_t d = 0; d < Distances.size(); d++)
        {
            size_t i = TopologicalSort[d];
            for (size_t AdjIndex : AdjacencyList[i])
            {
                if (Distances[AdjIndex] < (Distances[i] + 1))
                {
                    Distances[AdjIndex] = Distances[i] + 1;
                }
            }
        }

        size_t Size = std::ranges::max(Distances) + 1;

        DepepndencyLevelListType Dependencies(Size);
        for (size_t i = 0; i < m_Passes.size(); ++i)
        {
            size_t Level       = Distances[i];
            auto&  CurBuilder  = Builders[i];
            auto&  CurResolver = Builders[i].RgResolver;

            Dependencies[Level].AddPass(
                RgContext,
                m_Passes[i].get(),
                std::move(CurResolver.m_RenderTargets),
                std::move(CurResolver.m_DepthStencil),
                std::move(CurResolver.m_ResourcesCreated),
                CurResolver.m_ResourceStates,
                CurResolver.m_TextureLayouts);
        }

        return Dependencies;
    }
} // namespace Ame::Gfx::RG