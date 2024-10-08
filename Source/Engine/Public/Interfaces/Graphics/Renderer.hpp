#pragma once

#include <Interfaces/Graphics/Config.hpp>
#include <Interfaces/Graphics/Signals.hpp>

namespace Ame
{
    namespace Ecs
    {
        class World;
    } // namespace Ecs
    namespace Rhi
    {
        class IRhiDevice;
        class IImGuiRenderer;
    } // namespace Rhi
} // namespace Ame

namespace Ame::Interfaces
{
    // {1EE289FB-1A56-4AC1-9D2B-7CF354B9A9BE}
    inline constexpr UId IID_Renderer{ 0x1ee289fb, 0x1a56, 0x4ac1, { 0x9d, 0x2b, 0x7c, 0xf3, 0x54, 0xb9, 0xa9, 0xbe } };

    struct RendererCreateDesc
    {
        Rhi::IRhiDevice*     RhiDevice     = nullptr;
        Ecs::World*          World         = nullptr;
        Rhi::IImGuiRenderer* ImguiRenderer = nullptr;
    };

    class AME_ENGINE_API IRenderer : public IObjectWithCallback
    {
    public:
        using IObjectWithCallback::IObjectWithCallback;

        /// <summary>
        /// Update the renderer
        /// </summary>
        /// <returns>True if the application should continue running</returns>
        virtual bool Tick() = 0;

    public:
        [[nodiscard]] uint32_t GetSyncInterval() const noexcept
        {
            return m_SyncInterval;
        }

        void SetSyncInterval(uint32_t syncInterval) noexcept
        {
            m_SyncInterval = syncInterval;
        }

        [[nodiscard]] const Math::Color4& GetClearColor() const noexcept
        {
            return m_ClearColor;
        }

        void SetClearColor(const Math::Color4& clearColor) noexcept
        {
            m_ClearColor = clearColor;
        }

    public:
        Signals::OnRenderBegin_Signal      OnRenderBegin;
        Signals::OnRenderUpdate_Signal     OnRenderUpdate;
        Signals::OnRenderPostUpdate_Signal OnRenderPostUpdate;
        Signals::OnRenderEnd_Signal        OnRenderEnd;

#ifndef AME_NO_IMGUI
        Signals::OnImGuiRender_Signal     OnImGuiRender;
        Signals::OnImGuiPostRender_Signal OnImGuiPostRender;
#endif

    protected:
        Math::Color4 m_ClearColor   = Colors::c_DimGray;
        uint32_t     m_SyncInterval = 0;
    };

    [[nodiscard]] AME_ENGINE_API Ptr<IRenderer> CreateRenderer(const RendererCreateDesc& createDesc);
} // namespace Ame::Interfaces