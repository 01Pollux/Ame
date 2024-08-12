#include <Editor/EditorApplication.hpp>
#include <Engine/Engine.hpp>

#include <Module/Rhi/RhiModule.hpp>
#include <Module/Graphics/GraphicsModule.hpp>
#include <Module/Editor/EditorModule.hpp>

//

#include <EcsComponent/Viewport/Camera.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>
#include <EcsComponent/Renderables/3D/ModelLoader.hpp>
#include <EcsComponent/Renderables/3D/StaticMesh.hpp>

#include <Module/Graphics/RendererSubmodule.hpp>
#include <Shading/Technique.hpp>
#include <Shading/Material.hpp>
#include <Core/Enum.hpp>
#include <Math/Common.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp>

#include <Module/Graphics/RendererSubmodule.hpp>
#include <Graphics/RenderGraph/Graphs/ForwardPlus.hpp>

#include <imgui.h>

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

        Ptr<Dg::IRenderDevice> renderDevice;
        GetEngine().GetRegistry().GetModule(IID_RhiModule)->QueryInterface(Dg::IID_RenderDevice, renderDevice.DblPtr<IObject>());

        Ptr<Rhi::CommonRenderPass> crp;
        GetEngine().GetRegistry().GetModule(IID_RhiModule)->QueryInterface(Rhi::IID_CommandRenderPass, crp.DblPtr<IObject>());

        Ptr<Gfx::Renderer> rendererSubmodule;
        GetEngine().GetRegistry().GetModule(IID_GraphicsModule)->QueryInterface(Gfx::IID_Renderer, rendererSubmodule.DblPtr<IObject>());

        //

        Ptr renderGraph{ ObjectAllocator<RG::Graph>()() };
        Gfx::RegisterForwardPlus(*renderGraph, world);

        Ecs::TransformComponent camTr;
        camTr.SetBasis({ { -0.0231232084, 0.00000000, -0.999730587 },
                         { -0.139856368, 0.990168571, 0.00323479460 },
                         { 0.989904404, 0.139892802, -0.0228955243 } });
        camTr.SetPosition({ -49.2009239, 92.5598831, 5.80854464 });

        auto cameraEntity = world->CreateEntity("Camera");
        cameraEntity->set(Ecs::CameraComponent{
            .RenderGraph = std::move(renderGraph) });
        cameraEntity->set(camTr);
        cameraEntity->set(Ecs::CameraOutputComponent{});

        //

        Rhi::MaterialCreateDesc materialDesc;

        Ptr material(Rhi::Material::Create(renderDevice, materialDesc));
        Ptr mdl(Ecs::MeshModelLoader::LoadModel({ .RenderDevice = renderDevice, .ModelPath = "Shared/Assets/Models/Sponza/sponza.obj" }));

        //

        for (auto idx : std::views::iota(0u, static_cast<uint32_t>(mdl->GetSubMeshes().size() - 1)))
        {
            Ptr submesh(ObjectAllocator<Ecs::StaticMesh>()(mdl, idx));

            auto meshEntity = world->CreateEntity("Mesh");
            meshEntity->set(Ecs::TransformComponent{});
            meshEntity->set(Ecs::StaticMeshComponent{ submesh });
        }

        rendererSubmodule->OnImGuiRender(
            [cameraEntity]
            {
                {
                    enum class CameraMoveType : uint8_t
                    {
                        None,

                        /// <summary>
                        /// To Activate: Hold RMB and use WASD to move the camera.
                        /// To move the camera, use WASD for forward, left, backward, and right.
                        /// To move the camera up and down, use Q and E.
                        /// To move the camera faster, hold Shift.
                        /// To move the camera slower, hold Ctrl.
                        /// To pan the camera, move the mouse.
                        /// </summary>
                        Fly,

                        /// <summary>
                        /// To Activate: Hold left alt + RMB.
                        /// </summary>
                        Orbit,

                        /// <summary>
                        /// To Activate: Hold left alt + LMB.
                        /// </summary>
                        Zoom
                    };

                    /// <summary>
                    /// Determine the camera move type.
                    /// </summary>
                    auto GetCameraMoveType = []() -> CameraMoveType
                    {
                        if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
                        {
                            if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
                            {
                                return CameraMoveType::Orbit;
                            }
                            else if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                            {
                                return CameraMoveType::Zoom;
                            }
                        }
                        else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
                        {
                            return CameraMoveType::Fly;
                        }
                        return CameraMoveType::None;
                    };

                    /// <summary>
                    /// Get the speed of the camera with modifiers.
                    /// </summary>
                    auto GetCameraKeyboardSpeed = []() -> float
                    {
                        constexpr float MinSpeed = 0.01f;
                        constexpr float MaxSpeed = 50.0f;

                        float Speed = 20.f;
                        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
                        {
                            Speed *= 2.0f;
                        }
                        else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
                        {
                            Speed *= 0.5f;
                        }

                        return std::clamp(Speed, MinSpeed, MaxSpeed);
                    };

                    /// <summary>
                    /// Get the speed of the camera with modifiers.
                    /// </summary>
                    auto GetCameraMouseSpeed = []() -> float
                    {
                        constexpr float MinSpeed = 0.01f;
                        constexpr float MaxSpeed = 50.0f;

                        float Speed = 0.5f;
                        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
                        {
                            Speed *= 2.0f;
                        }
                        else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
                        {
                            Speed *= 0.5f;
                        }

                        return std::clamp(Speed, MinSpeed, MaxSpeed);
                    };

                    /// <summary>
                    /// Process the camera keyboard inputs.
                    /// </summary>
                    auto ProcessCameraKeyboard = [&](
                                                     float                       DeltaTime,
                                                     Ecs::TransformComponent*    Transform,
                                                     const Ecs::CameraComponent& Camera) -> bool
                    {
                        float Speed = GetCameraKeyboardSpeed();

                        Math::Vector3 MoveDelta{};
                        bool          Changed = false;

                        if (ImGui::IsKeyDown(ImGuiKey_W))
                        {
                            MoveDelta += Transform->GetLookDir() * Speed;
                            Changed = true;
                        }
                        if (ImGui::IsKeyDown(ImGuiKey_S))
                        {
                            MoveDelta -= Transform->GetLookDir() * Speed;
                            Changed = true;
                        }

                        if (ImGui::IsKeyDown(ImGuiKey_A))
                        {
                            MoveDelta -= Transform->GetRightDir() * Speed;
                            Changed = true;
                        }
                        if (ImGui::IsKeyDown(ImGuiKey_D))
                        {
                            MoveDelta += Transform->GetRightDir() * Speed;
                            Changed = true;
                        }

                        if (ImGui::IsKeyDown(ImGuiKey_Q))
                        {
                            MoveDelta -= Transform->GetUpDir() * Speed;
                            Changed = true;
                        }
                        if (ImGui::IsKeyDown(ImGuiKey_E))
                        {
                            MoveDelta += Transform->GetUpDir() * Speed;
                            Changed = true;
                        }

                        if (Changed)
                        {
                            MoveDelta *= DeltaTime;
                            Transform->SetPosition(Transform->GetPosition() + MoveDelta);
                        }

                        return Changed;
                    };

                    //

                    /// <summary>
                    /// Process the camera orbit inputs.
                    /// </summary>
                    auto ProcessCamera_Orbit = [&](float                    DeltaTime,
                                                   Ecs::TransformComponent* Transform) -> bool
                    {
                        Math::Quaternion Rotation   = Transform->GetBasis().GetRotationQuat();
                        float            Speed      = GetCameraMouseSpeed();
                        auto             MouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

                        bool Changed = false;
                        if (MouseDelta.x)
                        {
                            Transform->AppendYaw(Math::Util::DegToRad(MouseDelta.x * Speed * DeltaTime));
                            Changed = true;
                        }
                        if (MouseDelta.y)
                        {
                            Transform->AppendPitch(Math::Util::DegToRad(MouseDelta.y * Speed * DeltaTime));
                            Changed = true;
                        }

                        return Changed;
                    };

                    //

                    /// <summary>
                    /// Process the camera orbit inputs.
                    /// </summary>
                    auto ProcessCamera_Zoom = [&](float                    DeltaTime,
                                                  Ecs::TransformComponent* Transform) -> bool
                    {
                        Math::Quaternion Rotation   = Transform->GetBasis().GetRotationQuat();
                        float            Speed      = GetCameraKeyboardSpeed();
                        auto             MouseDelta = ImGui::GetMouseDragDelta();

                        bool Changed = false;
                        if (float Length = Math::Vector2(MouseDelta.x, MouseDelta.y).Length() * (MouseDelta.x >= 0.f ? 1.f : -1.f))
                        {
                            Transform->SetPosition(Transform->GetPosition() + Transform->GetLookDir() * Length * Speed * DeltaTime);
                            Changed = true;
                        }

                        return Changed;
                    };

                    auto Transform = cameraEntity->get_mut<Ecs::TransformComponent>();
                    auto Camera    = cameraEntity->get<Ecs::CameraComponent>();

                    bool  Changed   = false;
                    float DeltaTime = 1.f / 60.f;

                    CameraMoveType MoveType = GetCameraMoveType();
                    switch (MoveType)
                    {
                    case CameraMoveType::Fly:
                        Changed |= ProcessCameraKeyboard(DeltaTime, Transform, *Camera);
                        [[fallthrough]];
                    case CameraMoveType::Orbit:
                        Changed |= ProcessCamera_Orbit(DeltaTime, Transform);
                        break;
                    case CameraMoveType::Zoom:
                        Changed |= ProcessCamera_Zoom(DeltaTime, Transform);
                        break;
                    }

                    if (Changed)
                    {
                        cameraEntity->modified<Ecs::TransformComponent>();
                    }
                }
            });
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
