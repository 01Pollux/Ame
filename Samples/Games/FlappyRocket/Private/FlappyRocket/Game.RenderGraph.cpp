#include <FlappyRocket/Engine.hpp>

#include <Gfx/RG/Passes/EntityCollectPass.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    namespace RG = Gfx::RG;

    void FlappyRocketGame::SetupRenderGraph(
        RG::Graph& RenderGraph)
    {
        auto& PassStorage = RenderGraph.GetPassStorage();

        PassStorage.NewPass<RG::Std::EntityCollectPass>(*m_EcsUniverse);
    }
} // namespace Ame::FlappyRocket
