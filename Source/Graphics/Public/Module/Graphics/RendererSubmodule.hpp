#pragma once

#include <Module/Submodule.hpp>
#include <Core/Signal.hpp>
#include <Math/Colors.hpp>

#include <Module/Graphics/Config.hpp>

namespace Ame::Signals
{
    AME_SIGNAL_DECL(OnRenderBegin, void());
    AME_SIGNAL_DECL(OnRenderUpdate, void());
    AME_SIGNAL_DECL(OnRenderPostUpdate, void());
    AME_SIGNAL_DECL(OnRenderEnd, void());

    AME_SIGNAL_DECL(OnImGuiRender, void());
    AME_SIGNAL_DECL(OnImGuiPostRender, void());
} // namespace Ame::Signals

namespace Diligent
{
    class ISwapChain;
    class IDeviceContext;
} // namespace Diligent

namespace Ame::Rhi
{
    class IRhiDevice;
    class IImGuiRenderer;
} // namespace Ame::Rhi

namespace Ame
{
    class RhiModule;

    class RendererSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_RendererSubmodule, IID_BaseSubmodule, Base);

    public:
        RendererSubmodule(
            IReferenceCounters*         counters,
            RhiModule*                  rhiModule,
            const GraphicsModuleConfig& config);

        ~RendererSubmodule() override;

        /// <summary>
        /// Returns true if the application should continue running.
        /// </summary>
        [[nodiscard]] bool Tick();

    public:
        [[nodiscard]] uint32_t GetSyncInterval() const noexcept
        {
            return m_SyncInterval;
        }

        void SetSyncInterval(
            uint32_t syncInterval) noexcept
        {
            m_SyncInterval = syncInterval;
        }

        [[nodiscard]] Math::Color4 GetClearColor() const noexcept
        {
            return m_ClearColor;
        }

        void SetClearColor(
            const Math::Color4& clearColor) noexcept
        {
            m_ClearColor = clearColor;
        }

    public:
        AME_SIGNAL_INST(OnRenderBegin);
        AME_SIGNAL_INST(OnRenderUpdate);
        AME_SIGNAL_INST(OnRenderPostUpdate);
        AME_SIGNAL_INST(OnRenderEnd);

#ifndef AME_DIST
        AME_SIGNAL_INST(OnImGuiRender);
        AME_SIGNAL_INST(OnImGuiPostRender);
#endif

    private:
        /// <summary>
        /// Clears the render target.
        /// </summary>
        void ClearRenderTarget();

    private:
        Ptr<Rhi::IRhiDevice>    m_RhiDevice;
        Ptr<Dg::IDeviceContext> m_DeviceContext;
        Ptr<Dg::ISwapChain>     m_Swapchain;
#ifndef AME_NO_IMGUI
        Ptr<Rhi::IImGuiRenderer> m_ImGuiRenderer;
#endif

        Math::Color4 m_ClearColor = Colors::c_DimGray;

        uint32_t m_SyncInterval = 0;
    };
} // namespace Ame