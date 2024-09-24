using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Marshallers.Log;

namespace AmeSharp.Core.Log.Streams;

public abstract class ICustomLoggerStream : ILoggerStream
{
    internal protected unsafe delegate void LoggerStreamCallback(LoggerInfoMarshaller.Unmanaged* message, IntPtr _);
    private readonly LoggerStreamCallback _loggerCallback;

    public ICustomLoggerStream() : base(LoggerStreamBridge.CreateCallback(), true)
    {
        unsafe
        {
            _loggerCallback = (data, _) => Write(LoggerInfoMarshaller.ConvertToManaged(*data));
        }
        LoggerStreamBridge.UpdateCallback(Handle, _loggerCallback, IntPtr.Zero);
    }

    public abstract void Write(LoggerInfo info);
}
