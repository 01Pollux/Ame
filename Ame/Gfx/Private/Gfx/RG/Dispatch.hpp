#pragma once

#include <Gfx/RG/Core.hpp>

namespace Ame::Gfx::RG
{
    class GraphicsSetup
    {
    public:
        GraphicsSetup(
            Rhi::CommandList&               commandList,
            const ResourceStorage&          resourceStorage,
            bool                            enable,
            std::span<const ResourceViewId> renderTargets,
            const ResourceViewId&           depthStencil);

        ~GraphicsSetup();

    private:
        /// <summary>
        /// Setup the render target for the graphics pipeline
        /// </summary>
        [[nodiscard]] std::vector<const Rhi::ResourceView*> SetupRenderTargets(
            const ResourceStorage&          resourceStorage,
            std::span<Rhi::ClearDesc>       clearDescs,
            std::span<const ResourceViewId> renderTargets) const;

        /// <summary>
        /// Setup the depth stencil for the graphics pipeline
        /// </summary>
        [[nodiscard]] const Rhi::ResourceView* SetupDepthStencil(
            const ResourceStorage&    resourceStorage,
            std::span<Rhi::ClearDesc> clearDescs,
            const ResourceViewId&     depthStencil) const;

    private:
        Rhi::CommandList* m_CommandList = nullptr;
    };
} // namespace Ame::Gfx::RG