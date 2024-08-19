#pragma once

#include <Rg/Resource.hpp>

namespace Ame::Rg
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

    public:
        /// <summary>
        /// Append render pass
        /// </summary>
        void AddPass(
            Pass*                       pass,
            std::vector<ResourceViewId> renderTargets,
            ResourceViewId              depthStencil,
            std::set<ResourceId>        resourceToCreate);

        /// <summary>
        /// Execute render passes
        /// </summary>
        void Execute(
            Context&                       context,
            std::span<Dg::IDeviceContext*> deviceContexts) const;

    private:
        /// <summary>
        /// Create resources that are needed for this level
        /// </summary>
        void LockStorage(
            Context& context) const;

        /// <summary>
        /// Execute render passes
        /// </summary>
        void ExecutePasses(
            Context&                       context,
            std::span<Dg::IDeviceContext*> deviceContexts) const;

        /// <summary>
        /// </summary>
        void UnlockStorage(
            Context& context) const;

    private:
        std::vector<RenderPassInfo> m_Passes;
        std::set<ResourceId>        m_ResourcesToCreate;
    };
} // namespace Ame::Rg