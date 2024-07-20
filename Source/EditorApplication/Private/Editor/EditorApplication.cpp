#include <Editor/EditorApplication.hpp>
#include <Engine/Engine.hpp>

#include <Module/Graphics/GraphicsModule.hpp>

namespace Ame
{
    void EditorApplication::OnLoad()
    {
        Base::OnLoad();

        auto& moduleRegistry = GetEngine().GetRegistry();
        moduleRegistry.RegisterModule<GraphicsModule>();
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
