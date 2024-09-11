#include <Plugin/EditorPlugin.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Interfaces/Rhi/ImGuiRenderer.hpp>

namespace Ame::Editor
{
    bool IEditorPlugin::OnPluginPreLoad(IModuleRegistry* registry)
    {
        Ptr<Interfaces::IImGuiRenderer> imguiRenderer;
        if (!IPlugin::OnPluginPreLoad(registry) ||
            !registry->RequestInterface(this, Interfaces::IID_ImGuiRenderer, imguiRenderer.DblPtr<IObject>()))
        {
            return false;
        }

        s_ModuleRegistry = registry;
        return true;
    }
} // namespace Ame::Editor
