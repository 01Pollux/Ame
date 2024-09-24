#pragma once

#include <EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

#include <Ecs/World.hpp>
#include <Ecs/Query.hpp>

namespace Ame::Ecs
{
    struct GlobalTransformComponent;
    struct CameraComponent;
    struct CameraOutputComponent;
} // namespace Ame::Ecs

namespace Ame::Editor
{
    class SceneViewEditorWindow : public IEditorWindow
    {
    private:
        using CameraQuery = Ecs::UniqueQuery<const Ecs::CameraComponent, const Ecs::GlobalTransformComponent,
                                             const Ecs::CameraOutputComponent>;

    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_SceneViewEditorWindow, IEditorWindow);

    private:
        IMPLEMENT_INTERFACE_CTOR(SceneViewEditorWindow);

    public:
        void OnDrawVisible() override;

    private:
        void RenderHeader();
        void RenderContent();

    private:
        void UpdateCameraList(Ecs::Iterator& iter);

        void ValidateCameraList();

    private:
        struct CameraInfo
        {
            Ecs::Entity Entity;
            String      Name;

            CameraInfo() : Name("None")
            {
            }

            CameraInfo(Ecs::Entity entity) : Entity(entity), Name(entity.GetName())
            {
            }
        };
        static inline CameraInfo s_NoneCamera;

    private:
        [[nodiscard]] const CameraInfo& GetCurrentCamera() const;

    private:
        Ptr<Ecs::World> m_World;
        CameraQuery     m_CameraQuery;

        std::vector<CameraInfo> m_CameraList;
        uint32_t                m_CurrentCameraIndex = 0;
    };
} // namespace Ame::Editor