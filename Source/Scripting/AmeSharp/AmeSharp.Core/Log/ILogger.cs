using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Log;

[Guid("940017D2-269C-45B6-803B-F3C530151CCA")]
public sealed class ILogger : IBaseObject
{
    private ILogger(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle, false) { }
    public static ILogger Reference(IntPtr handle) => new(handle, false);
    public static ILogger Create(string loggerName) => new(LoggerBridge.Create(loggerName), true);

    public static ILogger? Instance
    {
        get
        {
            var @ref = LoggerBridge.GetInstance();
            return IsHandleInvalid(@ref) ? null : Reference(@ref);
        }
        set
        {
            if (value is null)
            {
                LoggerBridge.SetInstance(InvalidHandle);
            }
            else
            {
                LoggerBridge.SetInstance(value.Handle);
            }
        }
    }

    public LogLevel Level
    {
        get => LoggerBridge.GetLevel(Handle);
        set => LoggerBridge.SetLevel(Handle, value);
    }

    public void Log(LogLevel level, string message) => LoggerBridge.WriteMessage(Handle, new() { Level = level, Message = message });

    public void Trace(string message)
    {
#if DEBUG
        Log(LogLevel.Trace, message);
#endif
    }
    public void Debug(string message)
    {
#if DEBUG
        Log(LogLevel.Debug, message);
#endif
    }
    public void Info(string message) => Log(LogLevel.Info, message);
    public void Warning(string message) => Log(LogLevel.Warning, message);
    public void Error(string message) => Log(LogLevel.Error, message);
    public void Fatal(string message) => Log(LogLevel.Fatal, message);

    public void Assert(bool condition, string message)
    {
        if (!condition)
        {
#if DEBUG
            Log(LogLevel.Error, message);
            System.Diagnostics.Debugger.Break();
#endif
        }
    }

    public void Validate(bool condition, string message)
    {
        if (!condition)
        {
            Log(LogLevel.Fatal, message);
            throw new InvalidOperationException(message);
        }
    }

    public static ILogger operator +(ILogger logger, ILoggerStream stream)
    {
        LoggerBridge.AddStream(logger.Handle, stream);
        return logger;
    }

    public static ILogger operator -(ILogger logger, ILoggerStream stream)
    {
        LoggerBridge.RemoveStream(logger.Handle, stream);
        return logger;
    }
}
