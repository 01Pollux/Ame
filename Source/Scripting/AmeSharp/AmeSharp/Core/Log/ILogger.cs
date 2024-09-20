using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Log;

[Guid("940017D2-269C-45B6-803B-F3C530151CCA")]
public class ILogger : IBaseObject
{
    public ILogger(IntPtr ptr) : base(ptr) { }
    public ILogger(string loggerName) : base(LoggerBridge.Create(loggerName)) { }

    private static ILogger _cachedLogger = new(IntPtr.Zero);
    public static ILogger? Instance
    {
        get
        {
            var ptr = LoggerBridge.GetInstance();
            if (_cachedLogger.NativePointer == IntPtr.Zero || _cachedLogger.NativePointer != ptr)
            {
                _cachedLogger = new(ptr);
            }
            return _cachedLogger;
        }
        set
        {
            if (value == null)
            {
                LoggerBridge.SetInstance(IntPtr.Zero);
                _cachedLogger = new(IntPtr.Zero);
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

    public void Trace(string message) => Log(LogLevel.Trace, message);
    public void Debug(string message) => Log(LogLevel.Debug, message);
    public void Info(string message) => Log(LogLevel.Info, message);
    public void Warning(string message) => Log(LogLevel.Warning, message);
    public void Error(string message) => Log(LogLevel.Error, message);
    public void Fatal(string message) => Log(LogLevel.Fatal, message);

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
