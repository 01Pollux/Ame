#include <Editor/EditorApplication.hpp>
#include <Engine/Engine.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Interfaces/Ecs/EntityWorld.hpp>
#include <Interfaces/Rhi/RhiDevice.hpp>
#include <Interfaces/Graphics/Renderer.hpp>

//

#include <EcsComponent/Math/Transform.hpp>
#include <EcsComponent/Viewport/Camera.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>
#include <EcsComponent/Renderables/3D/ModelLoader.hpp>
#include <EcsComponent/Renderables/3D/StaticMesh.hpp>
#include <EcsComponent/Lighting/DirectionalLight.hpp>
#include <EcsComponent/Scene/RuntimeScene.hpp>

#include <Shading/Technique.hpp>
#include <Shading/Material.hpp>
#include <Core/Enum.hpp>
#include <Math/Common.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp>
#include <Graphics/RenderGraph/Graphs/ForwardPlus.hpp>

#include <Log/Logger.hpp>
#include <imgui.h>

namespace Ame
{
    EditorApplication::EditorApplication(const EditorApplicationConfig& config) : Base(config.Application)
    {
    }

    void EditorApplication::OnLoad()
    {
        Base::OnLoad();

        //

        auto moduleRegistry = GetEngine().GetRegistry();
        moduleRegistry->LoadPlugin("EditorCore");

        //

        Ptr<Interfaces::IEntityWorld> entityWorld;
        moduleRegistry->RequestInterface(nullptr, Interfaces::IID_EntityWorld, entityWorld.DblPtr<IObject>());

        Ptr<Interfaces::IRhiDevice> rhiDevice;
        moduleRegistry->RequestInterface(nullptr, Interfaces::IID_RhiDevice, rhiDevice.DblPtr<IObject>());

        Ptr<Interfaces::IRenderer> renderer;
        moduleRegistry->RequestInterface(nullptr, Interfaces::IID_Renderer, renderer.DblPtr<IObject>());

        //

        auto scene = AmeCreate(Ecs::RuntimeScene, entityWorld);
        scene->SetCurrent();

        //

        Ptr renderGraph{ ObjectAllocator<Rg::Graph>()(rhiDevice) };
        Gfx::RegisterForwardPlus(*renderGraph, entityWorld);

        Ecs::TransformComponent camTr;
        camTr.SetBasis({
            { 0.999921024f, 0.f, 0.0130040040f },
            { -0.0129953651f, 0.0340723470f, 0.999358475f },
            { -0.000444319536f, -0.999437153f, 0.0340764970f },
        });
        camTr.SetPosition({ 0.251502007f, 4.27811623f, -0.114832222f });

        auto cameraEntity = scene->CreateEntity("Camera");
        cameraEntity->set(Ecs::CameraComponent{ .RenderGraph = std::move(renderGraph) });
        cameraEntity->set(camTr);
        cameraEntity->set(Ecs::CameraOutputComponent{});

        //

        auto lightEntity = scene->CreateEntity("Light");

        Ecs::TransformComponent lightTr;
        lightTr.SetPosition({ 0.f, 10.f, 0.f });

        Math::Matrix3x3 basis = lightTr.GetBasis();
        // Makr it face down
        basis(Math::TransformMatrix::LookDirIndex) = { 0.f, 0.f, -1.f };
        lightTr.SetBasis(basis);

        Ecs::DirectionalLightComponent lightComp;
        lightComp.Color = Math::Color4{ 1.f, 1.f, 1.f, 1.f };

        lightEntity->set(lightComp);
        lightEntity->set(lightTr);

        //

        Rhi::MaterialCreateDesc materialDesc;

        Ptr mdl(Ecs::MeshModelLoader::LoadModelAsync(
                    { .RhiDevice = rhiDevice, .ModelPath = "Shared/Assets/Models/DamagedHelmet/DamagedHelmet.gltf" })
                    .get());

        //

        for (auto idx : std::views::iota(0u, static_cast<uint32_t>(mdl->GetSubMeshes().size())))
        {
            Ptr submesh(ObjectAllocator<Ecs::StaticMesh>()(mdl, idx));

            auto meshEntity = scene->CreateEntity("Mesh");
            meshEntity->set(Ecs::TransformComponent{});
            meshEntity->set(Ecs::StaticMeshComponent{ submesh });
        }

        renderer->OnImGuiRender.ConnectEx(
            [cameraEntity](const Signals::Connection& con)
            {
                if (!cameraEntity)
                {
                    con.disconnect();
                    return;
                }

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
                        return CameraMoveType::Fly;
                    };

                    /// <summary>
                    /// Get the speed of the camera with modifiers.
                    /// </summary>
                    auto GetCameraKeyboardSpeed = []() -> float
                    {
                        constexpr float MinSpeed = 0.01f;
                        constexpr float MaxSpeed = 50.0f;

                        float Speed = 2.5f;
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

                        float Speed = 0.05f;
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
                    auto ProcessCameraKeyboard = [&](float DeltaTime, Ecs::TransformComponent* Transform,
                                                     const Ecs::CameraComponent&) -> bool
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
                    auto ProcessCamera_Orbit = [&](float DeltaTime, Ecs::TransformComponent* Transform) -> bool
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
                    auto ProcessCamera_Zoom = [&](float DeltaTime, Ecs::TransformComponent* Transform) -> bool
                    {
                        Math::Quaternion Rotation   = Transform->GetBasis().GetRotationQuat();
                        float            Speed      = GetCameraKeyboardSpeed();
                        auto             MouseDelta = ImGui::GetMouseDragDelta();

                        bool Changed = false;
                        if (float Length =
                                Math::Vector2(MouseDelta.x, MouseDelta.y).Length() * (MouseDelta.x >= 0.f ? 1.f : -1.f))
                        {
                            Transform->SetPosition(Transform->GetPosition() +
                                                   Transform->GetLookDir() * Length * Speed * DeltaTime);
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
