#include <EditorPlugin/Windows/SceneView/SceneViewEditorWindow.hpp>

#include <Module/Ecs/EntityModule.hpp>

#include <EcsComponent/Math/TransformComponent.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>
#include <EcsComponent/Viewport/Camera.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> SceneViewEditorWindow::Create(
        ModuleRegistry& registry)
    {
        return { ObjectAllocator<SceneViewEditorWindow>()(registry), IID_BaseEditorWindow };
    }

    SceneViewEditorWindow::SceneViewEditorWindow(
        IReferenceCounters* counter,
        ModuleRegistry&     registry) :
        Base(counter, registry, c_SceneViewEditorWindowPath),
        m_World(GetModuleRegistry().QueryInterface<Ecs::World>(IID_EntityModule, Ecs::IID_EntityWorld)),
        m_CameraQuery(m_World->CreateQuery<
                                 const Ecs::CameraComponent,
                                 const Ecs::GlobalTransformComponent,
                                 const Ecs::CameraOutputComponent>()
                          .order_by<Ecs::CameraComponent>([](
                                                              Ecs::EntityId, auto a,
                                                              Ecs::EntityId, auto b)
                                                          { return a->Priority - b->Priority; })
                          .build())
    {
    }

    void SceneViewEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };

        RenderHeader();
        ImGui::Separator();
        RenderContent();
    }

    //

    void SceneViewEditorWindow::RenderHeader()
    {
        if (m_CameraQuery->changed())
        {
            m_CameraQuery->run([this](auto& iter)
                               { UpdateCameraList(iter); });
        }

        ValidateCameraList();

        const float maxListWidth = 150;
        const float listPosition = ImGui::GetContentRegionAvail().x - maxListWidth;

        ImGui::SetNextItemWidth(maxListWidth);
        ImGui::SetCursorPosX(listPosition);

        auto& currentCamera = GetCurrentCamera();
        if (ImGui::BeginCombo("##CameraList", currentCamera.Name.c_str()))
        {
            if (ImGui::Selectable(s_NoneCamera.Name.c_str(), m_CurrentCameraIndex == m_CameraList.size()))
            {
                m_CurrentCameraIndex = m_CameraList.size();
            }

            ImGui::Separator();
            for (size_t i = 0; i < m_CameraList.size(); i++)
            {
                if (ImGui::Selectable(m_CameraList[i].Name.c_str(), m_CurrentCameraIndex == i))
                {
                    m_CurrentCameraIndex = i;
                }
            }

            ImGui::EndCombo();
        }
    }

    //

    void SceneViewEditorWindow::RenderContent()
    {
        auto& currentCamera = GetCurrentCamera();
        if (!currentCamera.Entity)
        {
            return;
        }

        auto cameraComponent = currentCamera.Entity->get<Ecs::CameraComponent>();
        auto cameraOutput    = currentCamera.Entity->get<Ecs::CameraOutputComponent>();
        if (!cameraComponent->RenderGraph) [[unlikely]]
        {
            return;
        }

        auto& graphStorage      = cameraComponent->RenderGraph->GetResourceStorage();
        auto  outputView        = graphStorage.GetResource(RG::ResourceId(cameraOutput->SourceView));
        auto  textureOutputView = outputView ? outputView->AsTexture() : nullptr;
        if (!textureOutputView) [[unlikely]]
        {
            return;
        }

        ImGui::Image(
            ImTextureID(textureOutputView->Resource->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE)),
            ImGui::GetContentRegionAvail());
    }

    //

    void SceneViewEditorWindow::UpdateCameraList(
        Ecs::Iterator& iter)
    {
        m_CameraList.clear();
        while (iter.next())
        {
            m_CameraList.reserve(m_CameraList.size() + iter.count());
            for (auto i : iter)
            {
                m_CameraList.emplace_back(iter.entity(i));
            }
        }
    }

    void SceneViewEditorWindow::ValidateCameraList()
    {
        std::erase_if(m_CameraList, [this](const CameraInfo& info)
                      { return !info.Entity; });
    }

    //

    auto SceneViewEditorWindow::GetCurrentCamera() const -> const CameraInfo&
    {
        return m_CurrentCameraIndex < m_CameraList.size() ? m_CameraList[m_CurrentCameraIndex] : s_NoneCamera;
    }
} // namespace Ame::Editor