#pragma once

#include <Rg/PassStorage.hpp>
#include <Rg/Context.hpp>

#include <Core/Signal.hpp>

namespace Ame::Rg
{
    class AME_ENGINE_API Graph : public IObjectWithCallback
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_RenderGraph, IObjectWithCallback);

    private:
        IMPLEMENT_INTERFACE_CTOR(Graph, Rhi::IRhiDevice* rhiDevice);

    public:
        /// <summary>
        /// Execute the render graph with the immediate context
        /// </summary>
        void Execute();

        /// <summary>
        /// Execute the render graph with the specified execution context
        /// </summary>
        void Execute(Dg::IDeviceContext* contexts);

    public:
        void               MarkDirty() noexcept;
        [[nodiscard]] bool IsDirty() const noexcept;

    public:
        /// <summary>
        /// Get the render graph builder
        /// </summary>
        [[nodiscard]] PassStorage& GetPassStorage();

        /// <summary>
        /// Get the render graph storage
        /// </summary>
        [[nodiscard]] const ResourceStorage& GetResourceStorage() const;

        /// <summary>
        /// Get the render graph storage
        /// </summary>
        [[nodiscard]] ResourceStorage& GetResourceStorage();

    private:
        Context     m_Context;
        PassStorage m_Passes;

        Signals::ScopedConnection m_OnWindowSizeChanged;
    };
} // namespace Ame::Rg