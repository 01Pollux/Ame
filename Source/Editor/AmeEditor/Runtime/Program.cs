using AmeEditor.Application;
using AmeEditor.Config;
using AmeSharp.Core.Config;
using AmeSharp.Core.Log;
using AmeSharp.Core.Log.Streams;
using AmeSharp.Rhi.Config;
using AmeSharp.Rhi.RhiDevice.Desc;
using AmeSharp.Rhi.Surface;

CoreConfig coreConfig = new();
coreConfig.Logger = new()
{
    DefaultLevel = LogLevel.Trace,
    LoggerStreams = [new IConsoleLoggerStream(), new IMsvcDebugLoggerStream(), new IFileLoggerStream("Logs/Editor.log")]
};

RhiConfig rhiConfig = new()
{
    DeviceDesc = new()
    {
        Types = [new D3D12RhiDeviceTypeDesc(), new D3D11RhiDeviceTypeDesc(), new VulkanRhiDeviceTypeDesc()],
        Surface = new() { Window = new IDesktopWindow(new() { Title = "Ame Editor" }) }
    }
};

EditorConfig config = new();
config.Application.Configs = [coreConfig, rhiConfig];

var application = new EditorApplication(config);
//application.Run();
