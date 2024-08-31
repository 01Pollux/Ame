#pragma once

#include <Rg/Resource.hpp>

namespace Ame::Rg
{
    class DependencyLevel
    {
        friend class Graph;
        friend class Context;

    public:
        /// <summary>
        /// Append render pass
        /// </summary>
        void AddPass(Pass* pass);

        /// <summary>
        /// Execute render passes
        /// </summary>
        void Execute(Context& context, Dg::IDeviceContext* deviceContext) const;

    private:
        /// <summary>
        /// Create resources that are needed for this level
        /// </summary>
        void LockStorage(Context& context) const;

        /// <summary>
        /// Execute render passes
        /// </summary>
        void ExecutePasses(Context& context, Dg::IDeviceContext* deviceContext) const;

        /// <summary>
        /// </summary>
        void UnlockStorage(Context& context) const;

    private:
        std::vector<Pass*> m_Passes;
    };
} // namespace Ame::Rg