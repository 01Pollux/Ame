#pragma once

namespace Ame::RG
{
    class Graph;
} // namespace Ame::RG

namespace Ame::Gfx
{
    /// <summary>
    /// Registers the Forward+ rendering pass to the graph.
    /// </summary>
    void RegisterForwardPlus(
        RG::Graph& graph);
}