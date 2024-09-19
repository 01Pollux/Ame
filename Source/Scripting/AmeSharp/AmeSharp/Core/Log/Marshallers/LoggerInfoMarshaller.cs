using AmeSharp.Core.Base.Marshallers;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Log.Marshallers;

[CustomMarshaller(typeof(LoggerInfo), MarshalMode.Default, typeof(LoggerInfoMarshaller))]
public static unsafe class LoggerInfoMarshaller
{
    public static LoggerInfoUnmanaged ConvertToUnmanaged(LoggerInfo managed)
    {
        return new()
        {
            Message = NativeStringViewMarshaller.ConvertToUnmanaged(managed.Message),
            Level = managed.Level
        };
    }

    public static LoggerInfo ConvertToManaged(LoggerInfoUnmanaged unmanaged)
    {
        return new LoggerInfo()
        {
            Message = NativeStringViewMarshaller.ConvertToManaged(unmanaged.Message),
            Level = unmanaged.Level
        };
    }

    public static void Free(LoggerInfoUnmanaged unmanaged)
    {
        NativeStringViewMarshaller.Free(unmanaged.Message);
    }
}
