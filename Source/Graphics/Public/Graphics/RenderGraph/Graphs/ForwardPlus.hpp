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

namespace Diligent
{
    class IRenderDevice;
} // namespace Diligent

namespace Ame::Gfx
{
    /// <summary>
    /// Registers the Forward+ rendering pass to the graph.
    /// </summary>
    void RegisterForwardPlus(
        Diligent::IRenderDevice* renderDevice,
        RG::Graph&               graph,
        Ecs::World*              world);
} // namespace Ame::Gfx