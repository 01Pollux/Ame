#pragma once

#include <Interfaces/Graphics/Renderer.hpp>
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

namespace Ame::Interfaces
{
    class RendererImpl : public IRenderer
    {
    private:
        using CameraRenderQuery = Ecs::UniqueQuery<const Ecs::GlobalTransformComponent, const Ecs::CameraComponent,
                                                   const Ecs::CameraOutputComponent>;

    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_Renderer, IRenderer);

    private:
        IMPLEMENT_INTERFACE_CTOR(RendererImpl, Rhi::IRhiDevice* rhiDevice, Ecs::World* world,
                                 Rhi::IImGuiRenderer* imguiRenderer);

    public:
        bool Tick() override;

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

        Gfx::EntityCompositor m_EntityCompositor;
        CameraRenderQuery     m_CameraQuery;
    };
} // namespace Ame::Interfaces