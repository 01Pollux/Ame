using AmeEditor.Application;
using AmeEditor.Config;
using AmeSharp.Core.Config;
using AmeSharp.Core.Log;
using AmeSharp.Core.Log.Streams;
using AmeSharp.Rhi.Config;
using AmeSharp.Rhi.RhiDevice.Desc;
using AmeSharp.Rhi.Surface;

{
    List<ILoggerStream> streams = [IConsoleLoggerStream.Create(), IMsvcDebugLoggerStream.Create(), IFileLoggerStream.Create("Logs/Editor.log")];
    CoreConfig coreConfig = new()
    {
        Logger = new()
        {
            DefaultLevel = LogLevel.Trace,
            LoggerStreams = streams
        }
    };

    using var window = IDesktopWindow.Create(new() { Title = "Ame Editor" });
    RhiConfig rhiConfig = new()
    {
        DeviceDesc = new()
        {
            Types = [new D3D12RhiDeviceTypeDesc(), new D3D11RhiDeviceTypeDesc(), new VulkanRhiDeviceTypeDesc()],
            Surface = new() { Window = window }
        }
    };

    EditorConfig config = new();
    config.Application.Configs = [coreConfig, rhiConfig];

    using var application = new EditorApplication(config);
    //application.Run();

    streams.ForEach(stream => stream.Dispose());
}
