using AmeEditor.Application;
using AmeEditor.Config;
using AmeSharp.Core.Log;
using AmeSharp.Core.Log.Streams;
using AmeSharp.Rhi.Device;

EditorConfig config = new();

config.Application.Engine.Core.Logger = new()
{
    DefaultLevel = LogLevel.Trace,
    LoggerStreams = [new IConsoleLoggerStream(), new IMsvcDebugLoggerStream(), new IFileLoggerStream("Logs/Editor.log")]
};

config.Application.Engine.Rhi = new()
{
    DeviceDesc = new()
    {
        CreateDescs = [new D3D12RhiDeviceCreateDesc(), new D3D11RhiDeviceCreateDesc(), new VulkanRhiDeviceCreateDesc()]
    }
};

var application = new EditorApplication(config);
application.Run();
