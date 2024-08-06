#include <Graphics/RenderGraph/Graphs/ForwardPlus.hpp>

#include <Graphics/RenderGraph/Passes/InitializeForwardPlus.hpp>
#include <Graphics/RenderGraph/Passes/RecordIndirectCommands.hpp>
#include <Graphics/RenderGraph/Passes/RenderIndirectCommands.hpp>

#include <RG/Graph.hpp>

namespace Ame::Gfx
{
    void RegisterForwardPlus(
        RG::Graph&  graph,
        Ecs::World* world)
    {
        auto& passStorage = graph.GetPassStorage();
        passStorage.NewPass<InitializeForwardPlusPass>("Forward+::Initialize");
        passStorage.NewPass<RecordIndirectCommandsPass>("Forward+::RecordIndirectCommands", world);
        passStorage.NewPass<RenderIndirectCommandsPass>("Forward+::RenderIndirectCommands");
    }
} // namespace Ame::Gfx