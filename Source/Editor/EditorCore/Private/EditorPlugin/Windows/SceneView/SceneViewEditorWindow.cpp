#include <EditorPlugin/Windows/SceneView/SceneViewEditorWindow.hpp>
#include <EditorCore/ModuleRegistryUtils.hpp>

#include <EcsComponent/Math/Transform.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>
#include <EcsComponent/Viewport/Camera.hpp>
#include <EcsComponent/Scene/SceneEntity.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    SceneViewEditorWindow::SceneViewEditorWindow(IReferenceCounters* counter) :
        Base(counter, SceneViewEditorWindowPath), m_World(ModuleUtils::GetWorld()),
        m_CameraQuery(m_World
                          ->CreateQuery<const Ecs::CameraComponent, const Ecs::GlobalTransformComponent,
                                        const Ecs::CameraOutputComponent>()
                          .with<Ecs::ActiveSceneEntityTag>()
                          .order_by<Ecs::CameraComponent>([](Ecs::EntityId, auto a, Ecs::EntityId, auto b)
                                                          { return a->Priority - b->Priority; })
                          .build())
    {
    }

    void SceneViewEditorWindow::OnDrawVisible()
    {
        imcxx::window window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };

        RenderHeader();
        ImGui::Separator();
        RenderContent();
    }

    //

    void SceneViewEditorWindow::RenderHeader()
    {
        if (m_CameraQuery->changed())
        {
            m_CameraQuery->run([this](auto& iter) { UpdateCameraList(iter); });
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
                m_CurrentCameraIndex = static_cast<uint32_t>(m_CameraList.size());
            }

            ImGui::Separator();
            for (uint32_t i = 0; i < static_cast<uint32_t>(m_CameraList.size()); i++)
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
        auto  outputView        = graphStorage.GetResource(Rg::ResourceId(cameraOutput->SourceView));
        auto  textureOutputView = outputView ? outputView->AsTexture() : nullptr;
        if (!textureOutputView) [[unlikely]]
        {
            return;
        }

        auto srv = textureOutputView->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE);
        if (srv)
        {
            ImGui::Image(ImTextureID(srv), ImGui::GetContentRegionAvail());
        }
    }

    //

    void SceneViewEditorWindow::UpdateCameraList(Ecs::Iterator& iter)
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
        std::erase_if(m_CameraList, [this](const CameraInfo& info) { return !info.Entity; });
    }

    //

    auto SceneViewEditorWindow::GetCurrentCamera() const -> const CameraInfo&
    {
        return m_CurrentCameraIndex < m_CameraList.size() ? m_CameraList[m_CurrentCameraIndex] : s_NoneCamera;
    }
} // namespace Ame::Editor