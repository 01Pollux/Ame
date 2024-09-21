using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Log;

[Guid("940017D2-269C-45B6-803B-F3C530151CCA")]
public sealed class ILogger : IBaseObject
{
    public ILogger(IntPtr obj) : base(obj) { }
    public ILogger(string loggerName) : base(LoggerBridge.Create(loggerName)) { }

    private static ILogger? _cachedLogger;
    public static ILogger? Instance
    {
        get
        {
            var ptr = LoggerBridge.GetInstance();
            if (_cachedLogger is null || _cachedLogger.NativePointer != ptr)
            {
                _cachedLogger = Get<ILogger>(ptr);
            }
            return _cachedLogger;
        }
        set
        {
            if (value == null)
            {
                LoggerBridge.SetInstance(IntPtr.Zero);
                _cachedLogger = null;
            }
            else
            {
                LoggerBridge.SetInstance(value.NativePointer);
                _cachedLogger = value;
            }
        }
    }

    public LogLevel Level
    {
        get => LoggerBridge.GetLevel(NativePointer);
        set => LoggerBridge.SetLevel(NativePointer, value);
    }

    public void Log(LogLevel level, string message) => LoggerBridge.WriteMessage(NativePointer, new() { Level = level, Message = message });

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
        LoggerBridge.AddStream(logger.NativePointer, stream.NativePointer);
        return logger;
    }

    public static ILogger operator -(ILogger logger, ILoggerStream stream)
    {
        LoggerBridge.RemoveStream(logger.NativePointer, stream.NativePointer);
        return logger;
    }
}
