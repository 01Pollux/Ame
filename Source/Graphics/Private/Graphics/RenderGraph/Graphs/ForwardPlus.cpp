#include <Graphics/RenderGraph/Graphs/ForwardPlus.hpp>

#include <Graphics/RenderGraph/Passes/Initializers/ForwardPlus_Initialize.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityEmptyVertexBuffers.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityResourceSignature.hpp>
#include <Graphics/RenderGraph/Passes/Rendering/DepthPrePass.hpp>
#include <Graphics/RenderGraph/Passes/Lighting/ComputeLightCull.hpp>
#include <Graphics/RenderGraph/Passes/Rendering/ForwardPlus_RenderObjects.hpp>

#include <Rg/Graph.hpp>

namespace Ame::Gfx
{
    void RegisterForwardPlus(
        Rg::Graph&  graph,
        Ecs::World* world)
    {
        auto& passStorage = graph.GetPassStorage();
        passStorage.NewPass<ForwardPlus_InitializePass>("Forward+::Initialize");
        passStorage.NewPass<EntityResourceSignature_GraphicsPass>("Forward+::Initialize_ERS");
        passStorage.NewPass<EntityEmptyVertexBuffersPass>("Forward+::Initialize_EEVB");
        passStorage.NewPass<DepthPrePass>("Forward+::DepthPrepass", world);
        passStorage.NewPass<ComputeLightCullPass>("Forward+::Compute Light Cull");
        passStorage.NewPass<ForwardPlus_RenderObjectsPass>("Forward+::Render Objects", world);
    }
} // namespace Ame::Gfx