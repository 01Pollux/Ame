using AmeSharp.Core.Base.Types;

namespace AmeSharp.Core.Log;

public record LogData(string Message, uint ThreadId, DateTime Time, LogLevel Level)
{
}

public struct LogDataUnmanaged
{
    public NativeStringViewUnmanaged Message;
    public uint ThreadId;
    public long TimePointInMs;
    public LogLevel Level;
}
