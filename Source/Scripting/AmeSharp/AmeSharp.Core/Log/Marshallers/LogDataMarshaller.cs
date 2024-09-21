using AmeSharp.Core.Base.Marshallers;
using AmeSharp.Core.Log;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Log.Marshallers;

[CustomMarshaller(typeof(LogData), MarshalMode.Default, typeof(LogDataMarshaller))]
public static unsafe class LogDataMarshaller
{
    public struct Unmanaged
    {
        public NativeStringViewMarshaller.Unmanaged Message;
        public uint ThreadId;
        public long TimePointInMs;
        public LogLevel Level;
    }

    public static Unmanaged ConvertToUnmanaged(LogData managed)
    {
        return new()
        {
            Message = NativeStringViewMarshaller.ConvertToUnmanaged(managed.Message),
            ThreadId = managed.ThreadId,
            TimePointInMs = managed.Time.ToUnixTimeMilliseconds(),
            Level = managed.Level
        };
    }

    public static LogData ConvertToManaged(Unmanaged unmanaged)
    {
        return new(
            Message: NativeStringViewMarshaller.ConvertToManaged(unmanaged.Message),
            ThreadId: unmanaged.ThreadId,
            Time: DateTimeOffset.FromUnixTimeMilliseconds(unmanaged.TimePointInMs),
            Level: unmanaged.Level
        );
    }

    public static void Free(Unmanaged unmanaged)
    {
        NativeStringViewMarshaller.Free(unmanaged.Message);
    }
}
