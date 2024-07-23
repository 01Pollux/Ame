#include <Editor/EditorApplication.hpp>
#include <Engine/Engine.hpp>

#include <Module/Editor/EditorModule.hpp>

namespace Ame
{
    EditorApplication::EditorApplication(
        const EditorApplicationConfig& config) :
        Base(config.Application)
    {
        auto& moduleRegistry = GetEngine().GetRegistry();
        moduleRegistry.RegisterModule<EditorModule>(config.EditorConfig);
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
