using AmeSharp.Core.Base.Marshallers;
using AmeSharp.Core.Log;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Log.Marshallers;

[CustomMarshaller(typeof(LoggerInfo), MarshalMode.Default, typeof(LoggerInfoMarshaller))]
public static unsafe class LoggerInfoMarshaller
{
    public unsafe struct Unmanaged
    {
        public NativeStringViewMarshaller.Unmanaged Message;
        public LogLevel Level;
    }

    public static Unmanaged ConvertToUnmanaged(LoggerInfo managed)
    {
        return new()
        {
            Message = NativeStringViewMarshaller.ConvertToUnmanaged(managed.Message),
            Level = managed.Level
        };
    }

    public static LoggerInfo ConvertToManaged(Unmanaged unmanaged)
    {
        return new LoggerInfo()
        {
            Message = NativeStringViewMarshaller.ConvertToManaged(unmanaged.Message),
            Level = unmanaged.Level
        };
    }

    public static void Free(Unmanaged unmanaged)
    {
        NativeStringViewMarshaller.Free(unmanaged.Message);
    }
}
