#include <Editor/EditorApplication.hpp>
#include <Engine/Engine.hpp>

#include <Module/Rhi/RhiModule.hpp>
#include <Module/Graphics/GraphicsModule.hpp>
#include <Module/Editor/EditorModule.hpp>

//

#include <EcsComponent/Viewport/Camera.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>

#include <Module/Graphics/RendererSubmodule.hpp>
#include <EcsComponent/Renderables/3D/ModelLoader.hpp>
#include <Shading/Technique.hpp>
#include <Shading/Material.hpp>
#include <Core/Enum.hpp>
#include <Math/Common.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp>

namespace Ame
{
    EditorApplication::EditorApplication(
        const EditorApplicationConfig& config) :
        Base(config.Application)
    {
        auto& moduleRegistry = GetEngine().GetRegistry();

        EditorModule::Dependencies deps{
            &moduleRegistry,
            moduleRegistry.GetModule<RhiModule>(IID_RhiModule),
            moduleRegistry.GetModule<GraphicsModule>(IID_GraphicsModule)
        };
        moduleRegistry.RegisterModule<EditorModule>(deps, config.EditorConfig);
    }

    void EditorApplication::OnLoad()
    {
        Base::OnLoad();

        //

        Ptr<Ecs::World> world;
        GetEngine().GetRegistry().GetModule<EditorModule>(IID_EntityModule)->QueryInterface(Ecs::IID_EntityWorld, world.DblPtr<IObject>());

        //

        Ptr renderGraph{ ObjectAllocator<RG::Graph>()() };

        {
            auto& passStorage = renderGraph->GetPassStorage();
            passStorage
                .NewTypedPass<void>("Clear Only")
                .Flags(RG::PassFlags::Graphics)
                .Build([](RG::Resolver& resolver)
                       {
                        RG::ResourceId simpleTexture("");
                        auto desc = resolver.GetBackbufferDesc();
                        desc.BindFlags |= Dg::BIND_FLAGS::BIND_SHADER_RESOURCE;
                        resolver.CreateTexture(simpleTexture, desc);

                        RG::RenderTargetViewDesc rtvDesc;
                        rtvDesc.ViewType = Dg::TEXTURE_VIEW_RENDER_TARGET;
                        rtvDesc.ClearColor = Colors::c_DarkRed;
                        rtvDesc.ClearType  = RG::ERTClearType::Color;
                        rtvDesc.ForceColor = true;

                        resolver.WriteTexture(
                            simpleTexture("Backbuffer"),
                            Dg::BIND_RENDER_TARGET,
                            rtvDesc
                        ); });
        }

        auto cameraEntity = world->CreateEntity("Camera");
        cameraEntity->set(Ecs::CameraComponent{
            .RenderGraph = std::move(renderGraph) });
        cameraEntity->set(Ecs::TransformComponent{});
        cameraEntity->set(Ecs::CameraOutputComponent{});
    }

    void EditorApplication::OnInitialize()
    {
        Base::OnInitialize();
    }

    void EditorApplication::OnShutdown()
    {
        Base::OnShutdown();
    }

    void EditorApplication::OnUnload()
    {
        Base::OnUnload();
    }
} // namespace Ame
