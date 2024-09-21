using AmeSharp.Core.Base;
using AmeSharp.Core.Log;
using AmeSharp.Core.Plugin;

namespace AmeSharp.Core.Config;

public class LoggerCoreConfig
{
    public string LoggerName = "Ame";
    public List<ILoggerStream>? LoggerStreams;
    public LogLevel DefaultLevel = LogLevel.Info;
    public bool SetAsMain = true;

    public void RegisterInterface(IModuleRegistry registry, IPlugin? owner = null)
    {
        if (LoggerName.Length != 0)
        {
            ILogger logger = new(LoggerName)
            {
                Level = DefaultLevel
            };
            LoggerStreams?.ForEach(stream => logger += stream);
            registry.ExposeInterface(owner, typeof(ILogger).GUID, logger);
            if (SetAsMain)
            {
                ILogger.Instance = logger;
            }
        }
    }
}

public class CoreConfig
{
    public LoggerCoreConfig? Logger = null;
    public bool EnableFrameTimer = true;
    public bool EnableFrameEvent = true;

    public void RegisterInterface(IModuleRegistry registry, IPlugin? owner = null)
    {
        Logger?.RegisterInterface(registry, owner);
        if (EnableFrameTimer)
        {
            registry.ExposeInterface(owner, typeof(IFrameTimer).GUID, new IFrameTimer());
        }
        if (EnableFrameEvent)
        {
            registry.ExposeInterface(owner, typeof(IFrameEvent).GUID, new IFrameEvent());
        }
    }
}
