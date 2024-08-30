#pragma once

#include <Graphics/Renderer/Signals.hpp>
#include <Graphics/EntityCompositor/EntityCompositor.hpp>

#include <Rhi/Device/RhiDevice.hpp>
#include <Rhi/ImGui/ImGuiRenderer.hpp>
#include <CommonStates/CommonRenderPasses.hpp>

#include <Ecs/World.hpp>
#include <Ecs/Query.hpp>

namespace Ame::Ecs
{
    struct GlobalTransformComponent;
    struct CameraComponent;
    struct CameraOutputComponent;
} // namespace Ame::Ecs

namespace Ame::Gfx
{
    // {71A7DA56-51B4-47BB-B354-D7F37502879E}
    inline constexpr UId IID_Renderer{ 0x71a7da56, 0x51b4, 0x47bb, { 0xb3, 0x54, 0xd7, 0xf3, 0x75, 0x2, 0x87, 0x9e } };

    class Renderer : public BaseObject<IObject>
    {
    private:
        using CameraRenderQuery = Ecs::UniqueQuery<
            const Ecs::GlobalTransformComponent,
            const Ecs::CameraComponent,
            const Ecs::CameraOutputComponent>;

    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_Renderer, Base);

    public:
        Renderer(
            IReferenceCounters*  counters,
            Rhi::IRhiDevice*     rhiDevice,
            Ecs::World*          world,
            Rhi::IImGuiRenderer* imguiRenderer);

    public:
        /// <summary>
        /// Returns true if the application should continue running. (the window is not closed)
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
        void CreateCameraQuery();
        void RunRenderGraph();

        void ClearRenderTarget();

    private:
        Ptr<Rhi::IRhiDevice>       m_RhiDevice;
        Ptr<Rhi::CommonRenderPass> m_CommonRenderPass;
        Ptr<Ecs::World>            m_World;
#ifndef AME_NO_IMGUI
        Ptr<Rhi::IImGuiRenderer> m_ImGuiRenderer;
#endif

        Math::Color4 m_ClearColor   = Colors::c_DimGray;
        uint32_t     m_SyncInterval = 0;

        EntityCompositor  m_EntityCompositor;
        CameraRenderQuery m_CameraQuery;
    };
} // namespace Ame::Gfx