using AmeEditor.Application;
using AmeEditor.Config;
using AmeSharp.Core.Log;
using AmeSharp.Core.Log.Streams;

EditorConfig config = new();

config.Application.Engine.Core.Logger = new()
{
    DefaultLevel = LogLevel.Trace,
    LoggerStreams = [new IConsoleLoggerStream(), new IMsvcDebugLoggerStream(), new IFileLoggerStream("Logs/Editor.log")]
};

var application = new EditorApplication(config);
application.Run();
