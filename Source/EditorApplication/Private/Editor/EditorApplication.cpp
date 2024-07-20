#include <Editor/EditorApplication.hpp>

#include <Engine/Engine.hpp>
#include <Module/Core/FrameEventSubmodule.hpp>

#include <Window/DesktopWindow.hpp>
#include <Module/Graphics/RhiGraphicsSubmodule.hpp>

namespace Ame
{
    EditorApplication::EditorApplication(
        const EditorApplicationConfig& editorConfig) :
        Base(editorConfig.Application)
    {
        auto& moduleRegistry = GetEngine().GetRegistry();

        Ptr<FrameEventSubmodule> frameEvent;
        moduleRegistry.GetModule(IID_CoreModule)->QueryInterface(IID_FrameEventSubmodule, frameEvent.DblPtr<IObject>());

        Ptr<Window::IDesktopWindow> dekstopWindow;
        moduleRegistry.GetModule(IID_GraphicsModule)->QueryInterface(Window::IID_DesktopWindow, dekstopWindow.DblPtr<IObject>());

        frameEvent->OnFrameStart(
            [this, dekstopWindow]()
            {
                dekstopWindow->ProcessEvents();

                if (!dekstopWindow->IsRunning())
                {
                    GetEngine().Exit(0);
                }
            });
    }

    void EditorApplication::OnLoad()
    {
        Base::OnLoad();
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
