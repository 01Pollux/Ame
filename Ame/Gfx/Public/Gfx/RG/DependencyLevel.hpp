#pragma once

#include <Gfx/RG/Resource.hpp>
#include <Rhi/Hash/View.hpp>

namespace Ame::Gfx::RG
{
    class DependencyLevel
    {
        friend class Graph;
        friend class Context;

        struct RenderPassInfo
        {
            Pass*                       RgPass;
            std::vector<ResourceViewId> RenderTargets;
            ResourceViewId              DepthStencil;
        };

        using TextureSubresourceTransitionStateMap = std::unordered_map<Rhi::TextureSubresource, nri::AccessLayoutStage>;
        using TextureTransitionStateMap            = std::map<ResourceId, TextureSubresourceTransitionStateMap>;
        using BufferTransitionStateMap             = std::map<ResourceId, nri::AccessStage>;

    public:
        /// <summary>
        /// Append render pass
        /// </summary>
        void AddPass(
            Context&                                          RgContext,
            Pass*                                             RgPass,
            std::vector<ResourceViewId>                       RenderTargets,
            ResourceViewId                                    DepthStencil,
            std::set<ResourceId>                              ResourceToCreate,
            const std::map<ResourceViewId, Rhi::AccessStage>& ResourceStates,
            const std::map<ResourceId, Rhi::LayoutType>&      TextureLayouts);

        /// <summary>
        /// Execute render passes
        /// </summary>
        void Execute(
            Context&          RgContext,
            Rhi::CommandList& CommandList) const;

    private:
        /// <summary>
        /// Create resources that are needed for this level
        /// </summary>
        void LockStorage(
            Context& RgContext) const;

        /// <summary>
        /// Execute pending resource barriers before render passes
        /// </summary>
        void ExecuteBarriers(
            Context&          RgContext,
            Rhi::CommandList& CommandList) const;

        /// <summary>
        /// Execute render passes
        /// </summary>
        void ExecutePasses(
            Context&          RgContext,
            Rhi::CommandList& CommandList) const;

        /// <summary>
        /// </summary>
        void UnlockStorage(
            Context& RgContext) const;

    private:
        std::vector<RenderPassInfo> m_Passes;

        std::set<ResourceId>      m_ResourcesToCreate;
        BufferTransitionStateMap  m_BufferStatesToTransitions;
        TextureTransitionStateMap m_TextureStatesToTransitions;
    };
} // namespace Ame::Gfx::RG