#include <Application/EntryPoint.hpp>
#include <Editor/EditorApplication.hpp>

static Ame::EditorApplicationConfig ParseEditorConfig(int, char**)
{
    using namespace Ame;

    Rhi::DeviceCreateDesc rhiDeviceDesc{
        .Types{ Rhi::DeviceCreateDescD3D12{}, Rhi::DeviceCreateDescVulkan{},
                Rhi::DeviceCreateDescD3D11{} }, // OpenGL is not supported as structured buffer and combined texture is
                                                // too much hassle
        .Surface = Rhi::RenderSurfaceDesc{ Window::CreateWindow(
            Window::WindowType::DesktopWindow,
            { .Title = "Ame Editor", .Size = { 1280, 720 }, .CustomTitleBar = true }) }
    };

    EditorApplicationConfig editorConfig;

    auto& loggerStreams = editorConfig.Application.Engine.CoreConfig.LoggerDesc.Streams;
    loggerStreams.emplace_back(std::make_unique<Ame::Log::FileStream>("Logs/Editor.log"));
    loggerStreams.emplace_back(std::make_unique<Ame::Log::MsvcDebugStream>());
    loggerStreams.emplace_back(std::make_unique<Ame::Log::ConsoleStream>());

    editorConfig.Application.Engine.RhiConfig =
        Interfaces::RhiModuleConfig{ .RhiDeviceDesc = std::move(rhiDeviceDesc) };

    editorConfig.Application.Engine.EcsConfig.emplace();
    editorConfig.Application.Engine.GraphicsConfig.emplace();

    return editorConfig;
}

AME_MAIN_APPLICATION_CONFIG(Ame::EditorApplication, ParseEditorConfig(argc, argv));
