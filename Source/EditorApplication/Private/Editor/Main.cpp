#include <Application/EntryPoint.hpp>
#include <Editor/EditorApplication.hpp>

static Ame::EditorApplicationConfig ParseEditorConfig(
    int    argc,
    char** argv)
{
    using namespace Ame;

    Rhi::DeviceCreateDesc rhiDeviceDesc{
        .Types{
            Rhi::DeviceCreateDescD3D12{},
            Rhi::DeviceCreateDescVulkan{},
            Rhi::DeviceCreateDescD3D11{},
            Rhi::DeviceCreateDescGL{} },
        .Surface = Rhi::RenderSurfaceDesc{
            Window::CreateWindow(Window::WindowType::DesktopWindow, { .Title = "Ame Editor" }) }
    };

    EditorApplicationConfig editorConfig;

    editorConfig.Application.Engine.RhiConfig = RhiModuleConfig{
        .RhiDeviceDesc = std::move(rhiDeviceDesc)
    };
    editorConfig.Application.Engine.GraphicsConfig.emplace();

    return editorConfig;
}

AME_MAIN_APPLICATION_CONFIG(Ame::EditorApplication, ParseEditorConfig(argc, argv));
