#include <Application/EntryPoint.hpp>
#include <Editor/Main.hpp>

#include <iostream>
#include <print>
#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

namespace Ame
{
    static bpo::options_description GetProgramOptions(Window::WindowCreateDesc& windowCreateDesc,
                                                      EditorApplicationConfig&  editorConfig)
    {
        auto& projectConfig = editorConfig.ProjectConfig;

        bpo::options_description programOpts("Allowed options");
        programOpts.add_options()("help", "Produce help message")(

            "custom_titlebar,ctb",
            bpo::value<bool>()->default_value(true)->notifier([&](bool val) { windowCreateDesc.CustomTitleBar = val; }),
            "Run maximized")(

            "maximized,m",
            bpo::value<bool>()->default_value(false)->notifier([&](bool val) { windowCreateDesc.Maximized = val; }),
            "Run maximized")(

            "fullscreen",
            bpo::value<bool>()->default_value(false)->notifier([&](bool val) { windowCreateDesc.FullScreen = val; }),
            "Run in fullscreen mode")(

            "width,w",
            bpo::value<int>()->default_value(1280)->notifier([&](int val) { windowCreateDesc.Size.x() = val; }),
            "Set window width")(

            "height,h",
            bpo::value<int>()->default_value(720)->notifier([&](int val) { windowCreateDesc.Size.y() = val; }),
            "Set window height")(

            "start-in-middle,sim",
            bpo::value<bool>()->default_value(true)->notifier([&](bool val) { windowCreateDesc.StartInMiddle = val; }),
            "Start window in the middle of the screen")(

            "project-path,p",
            bpo::value<String>()->notifier([&](String val) { projectConfig->ProjectPath = std::move(val); }),
            "Project path (containing .neon file) to load")(

            "new-project-name,n",
            bpo::value<String>()->notifier([&](String val) { projectConfig->ProjectName = std::move(val); }),
            "Creates a new project with name");
        return programOpts;
    }

    bool EditorApplicationConfig::Parse(int argc, char** argv)
    {
        Window::WindowCreateDesc windowCreateDesc{ .Title          = "Ame Editor",
                                                   .Size           = { 1280, 720 },
                                                   .CustomTitleBar = true };

        auto               programOpts = GetProgramOptions(windowCreateDesc, *this);
        bpo::variables_map programVars;
        try
        {
            bpo::store(bpo::parse_command_line(argc, argv, programOpts), programVars);
            bpo::notify(programVars);
        }
        catch (const std::exception& ex)
        {
            std::println("Error parsing command line arguments: {}", ex.what());
            return false;
        }

        if (programVars.count("help"))
        {
            std::println("Usage: Editor [options]");
            std::cout << programOpts << std::endl;
            return false;
        }

        if (!ProjectConfig->IsValid())
        {
            std::println("Project path/Project name is not valid.");
            return false;
        }

        //

        Rhi::DeviceCreateDesc rhiDeviceDesc{
            .Types{ Rhi::DeviceCreateDescD3D12{}, Rhi::DeviceCreateDescVulkan{},
                    Rhi::DeviceCreateDescD3D11{} }, // OpenGL is not supported as structured buffer and combined texture
                                                    // is too much hassle
            .Surface = Rhi::RenderSurfaceDesc{ Window::CreateWindow(
                Window::WindowType::DesktopWindow, std::move(windowCreateDesc)) },
        };

        auto& loggerStreams = Application.Engine.CoreConfig.LoggerDesc.Streams;
        loggerStreams.emplace_back(std::make_unique<Log::FileStream>("Logs/Editor.log"));
        loggerStreams.emplace_back(std::make_unique<Log::MsvcDebugStream>());
        loggerStreams.emplace_back(std::make_unique<Log::ConsoleStream>());

        Application.Engine.RhiConfig = Interfaces::RhiModuleConfig{ .RhiDeviceDesc = std::move(rhiDeviceDesc) };

        Application.Engine.EcsConfig.emplace();
        Application.Engine.GraphicsConfig.emplace();

        return true;
    }

    Ame::EditorApplicationConfig ParseEditorConfig(int argc, char** argv)
    {
        EditorApplicationConfig editorConfig;
        if (!editorConfig.Parse(argc, argv))
        {
            std::exit(1);
        }
        return editorConfig;
    }
} // namespace Ame
