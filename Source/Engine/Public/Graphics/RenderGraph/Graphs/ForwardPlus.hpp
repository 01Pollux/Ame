#pragma once

namespace Ame
{
    namespace Rg
    {
        class Graph;
    } // namespace Rg
    namespace Ecs
    {
        class World;
    } // namespace Ecs
} // namespace Ame

namespace Ame::Gfx
{
    /// <summary>
    /// Registers the Forward+ rendering pass to the graph.
    /// </summary>
    AME_ENGINE_API void RegisterForwardPlus(Rg::Graph& graph, Ecs::World* world);
} // namespace Ame::Gfx