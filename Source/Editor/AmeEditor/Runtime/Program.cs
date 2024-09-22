using AmeEditor.Application;
using AmeEditor.Config;
using AmeSharp.Core.Log;
using AmeSharp.Core.Log.Streams;
using AmeSharp.Rhi.Surface;

{
    string title = "";
    using IWindow window = new IDesktopWindow(new() { Title = title });
    while (window.IsRunning)
    {
        // sleep for 16ms
        Thread.Sleep(16);
        title += "*";
        if (title.Length > 60)
        {
            title = "";
        }
        window.Title = title;
    }
}

EditorConfig config = new();

config.Application.Engine.Core.Logger = new()
{
    DefaultLevel = LogLevel.Trace,
    LoggerStreams = [new IConsoleLoggerStream(), new IMsvcDebugLoggerStream(), new IFileLoggerStream("Logs/Editor.log")]
};

//config.Application.Engine.Rhi = new()
//{
//    DeviceDesc = new()
//    {
//        CreateDescs = [new D3D12RhiDeviceCreateDesc(), new D3D11RhiDeviceCreateDesc(), new VulkanRhiDeviceCreateDesc()]
//    }
//};

var application = new EditorApplication(config);
application.Engine.Exit(0);
application.Run();
