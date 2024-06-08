#pragma once

#include <Rhi/Nri/Nri.hpp>
#include <Core/String.hpp>
#include <Math/Colors.hpp>

namespace Ame::Rhi::D3D12
{
    /// <summary>
    /// Begins a PIX3 marker region.
    /// </summary>
    void CommandList_BeginMarker(
        nri::CoreInterface& coreInterface,
        nri::CommandBuffer& commandList,
        const String&       label,
        const Math::Color4& color);

    /// <summary>
    /// Marks an event in a PIX3 marker region.
    /// </summary>
    void CommandList_MarkEvent(
        nri::CoreInterface& coreInterface,
        nri::CommandBuffer& commandList,
        const String&       label,
        const Math::Color4& color);

    /// <summary>
    /// Ends a PIX3 marker region.
    /// </summary>
    void CommandList_EndMarker(
        nri::CoreInterface& coreInterface,
        nri::CommandBuffer& commandList);
} // namespace Ame::Rhi::D3D12