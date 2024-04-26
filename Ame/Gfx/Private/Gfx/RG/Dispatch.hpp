#pragma once

#include <Gfx/RG/Core.hpp>

namespace Ame::Gfx::RG
{
    class GraphicsSetup
    {
    public:
        GraphicsSetup(
            Rhi::CommandList&               CommandList,
            const Storage&                  RgStorage,
            bool                            Enable,
            std::span<const ResourceViewId> RenderTargets,
            const ResourceViewId&           DepthStencil);

        ~GraphicsSetup();

    private:
        /// <summary>
        /// Setup the render target for the graphics pipeline
        /// </summary>
        [[nodiscard]] std::vector<const Rhi::ResourceView*> SetupRenderTargets(
            const Storage&                  RgStorage,
            std::span<Rhi::ClearDesc>       ClearDescs,
            std::span<const ResourceViewId> RenderTargets) const;

        /// <summary>
        /// Setup the depth stencil for the graphics pipeline
        /// </summary>
        [[nodiscard]] const Rhi::ResourceView* SetupDepthStencil(
            const Storage&            RgStorage,
            std::span<Rhi::ClearDesc> ClearDescs,
            const ResourceViewId&     DepthStencil) const;

    private:
        Rhi::CommandList* m_CommandList;
    };
} // namespace Ame::Gfx::RG