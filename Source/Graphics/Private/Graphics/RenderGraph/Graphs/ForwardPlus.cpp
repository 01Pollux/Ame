#include <Graphics/RenderGraph/Graphs/ForwardPlus.hpp>

#include <Graphics/RenderGraph/Passes/ForwardPlus_Initialize.hpp>
#include <Graphics/RenderGraph/Passes/Initialize_EntityResourceSignature.hpp>
#include <Graphics/RenderGraph/Passes/ForwardPlus_RenderObjects.hpp>

#include <RG/Graph.hpp>

namespace Ame::Gfx
{
    void RegisterForwardPlus(
        RG::Graph&  graph,
        Ecs::World* world)
    {
        auto& passStorage = graph.GetPassStorage();
        passStorage.NewPass<ForwardPlus_InitializePass>("Forward+::Initialize");
        passStorage.NewPass<Initialize_EntityResourceSignature_Graphics>("Forward+::InitializeERS");
        passStorage.NewPass<ForwardPlus_RenderObjects>("Forward+::Render Objects", world);
    }
} // namespace Ame::Gfx