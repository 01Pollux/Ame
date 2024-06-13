#pragma once

#include <RG/Resource.hpp>
#include <Rhi/Hash/View.hpp>

namespace Ame::RG
{
    class DependencyLevel
    {
        friend class Graph;
        friend class Context;

        struct RenderPassInfo
        {
            Pass*                       NodePass;
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
            Context&                                          context,
            Pass*                                             pass,
            std::vector<ResourceViewId>                       renderTargets,
            ResourceViewId                                    depthStencil,
            std::set<ResourceId>                              resourceToCreate,
            const std::map<ResourceViewId, Rhi::AccessStage>& resourceStates,
            const std::map<ResourceId, Rhi::LayoutType>&      textureLayouts);

        /// <summary>
        /// Execute render passes
        /// </summary>
        void Execute(
            Context&          context,
            Rhi::CommandList& commandList) const;

    private:
        /// <summary>
        /// Create resources that are needed for this level
        /// </summary>
        void LockStorage(
            Context& context) const;

        /// <summary>
        /// Execute pending resource barriers before render passes
        /// </summary>
        void ExecuteBarriers(
            Context&          context,
            Rhi::CommandList& commandList) const;

        /// <summary>
        /// Execute render passes
        /// </summary>
        void ExecutePasses(
            Context&          context,
            Rhi::CommandList& commandList) const;

        /// <summary>
        /// </summary>
        void UnlockStorage(
            Context& context) const;

    private:
        std::vector<RenderPassInfo> m_Passes;

        std::set<ResourceId>      m_ResourcesToCreate;
        BufferTransitionStateMap  m_BufferStatesToTransitions;
        TextureTransitionStateMap m_TextureStatesToTransitions;
    };
} // namespace Ame::RG