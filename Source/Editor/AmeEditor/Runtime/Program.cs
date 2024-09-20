using AmeEditor.Application;
using AmeEditor.Config;
using AmeSharp.Core.Log;
using AmeSharp.Core.Log.Streams;

EditorConfig config = new();

var logger = config.Application.Engine.Core.Logger = new();
logger.DefaultLevel = LogLevel.Trace;
logger.LoggerStreams = [new ConsoleStream(), new MsvcDebugStream()];

var application = new EditorApplication(config);
application.Run();
