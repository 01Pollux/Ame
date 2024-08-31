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
    struct RendererCreateDesc
    {
        Rhi::IRhiDevice*     RhiDevice     = nullptr;
        Ecs::World*          World         = nullptr;
        Rhi::IImGuiRenderer* ImguiRenderer = nullptr;
    };

    class IRenderer : public IObject
    {
    public:
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
        AME_SIGNAL_INST(OnRenderBegin);
        AME_SIGNAL_INST(OnRenderUpdate);
        AME_SIGNAL_INST(OnRenderPostUpdate);
        AME_SIGNAL_INST(OnRenderEnd);

#ifndef AME_DIST
        AME_SIGNAL_INST(OnImGuiRender);
        AME_SIGNAL_INST(OnImGuiPostRender);
#endif

    protected:
        Math::Color4 m_ClearColor   = Colors::c_DimGray;
        uint32_t     m_SyncInterval = 0;
    };

    [[nodiscard]] Ptr<IRenderer> CreateRenderer(const RendererCreateDesc& createDesc);
} // namespace Ame::Interfaces