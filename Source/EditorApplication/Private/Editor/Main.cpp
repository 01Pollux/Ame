#include <Application/EntryPoint.hpp>
#include <Editor/EditorApplication.hpp>

static Ame::EditorApplicationConfig ParseEditorConfig(
    int,
    char**)
{
    using namespace Ame;

    Log::Logger::Register(Log::Names::c_Engine, "Logs/Engine.log");
    Log::Logger::Register(Log::Names::c_Ecs, "Logs/Engine.log");
    Log::Logger::Register(Log::Names::c_Client, "Logs/Editor.log");
    Log::Logger::Register(Log::Names::c_Editor, "Logs/Editor.log");
    Log::Logger::Register(Log::Names::c_Asset, "Logs/Engine.log");
    Log::Logger::Register(Log::Names::c_Window, "Logs/Engine.log");
    Log::Logger::Register(Log::Names::c_Rhi, "Logs/Engine.log");
    Log::Logger::Register(Log::Names::c_Gfx, "Logs/Engine.log");

    Rhi::DeviceCreateDesc rhiDeviceDesc{
        .Types{
            Rhi::DeviceCreateDescD3D12{},
            Rhi::DeviceCreateDescVulkan{},
            Rhi::DeviceCreateDescD3D11{} }, // OpenGL is not supported as structured buffer and combined texture is too much hassle
        .Surface = Rhi::RenderSurfaceDesc{
            Window::CreateWindow(Window::WindowType::DesktopWindow,
                                 { .Title          = "Ame Editor",
                                   .Size           = { 1280, 720 },
                                   .CustomTitleBar = true }) }
    };

    EditorApplicationConfig editorConfig;

    editorConfig.Application.Engine.RhiConfig = RhiModuleConfig{
        .RhiDeviceDesc = std::move(rhiDeviceDesc)
    };

    editorConfig.Application.Engine.EcsConfig.emplace();
    editorConfig.Application.Engine.GraphicsConfig.emplace();

    return editorConfig;
}

AME_MAIN_APPLICATION_CONFIG(Ame::EditorApplication, ParseEditorConfig(argc, argv));
