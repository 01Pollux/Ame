#pragma once

namespace Ame
{
    namespace RG
    {
        class Graph;
    } // namespace RG
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
    void RegisterForwardPlus(
        RG::Graph&  graph,
        Ecs::World* world);
} // namespace Ame::Gfx