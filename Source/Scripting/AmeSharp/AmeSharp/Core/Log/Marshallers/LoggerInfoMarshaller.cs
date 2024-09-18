using AmeSharp.Core.Base.Marshallers;
using AmeSharp.Core.Log;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Log.Marshallers;

[CustomMarshaller(typeof(LoggerInfo), MarshalMode.Default, typeof(LoggerInfoMarshaller))]
internal static unsafe class LoggerInfoMarshaller
{
    internal struct UnmanagedLoggerInfo
    {
        public NativeStringViewMarshaller.UnmanagedView Message;
        public LogLevel Level;
    }

    public static UnmanagedLoggerInfo ConvertToUnmanaged(LoggerInfo managed)
    {
        return new UnmanagedLoggerInfo()
        {
            Message = NativeStringViewMarshaller.ConvertToUnmanaged(managed.Message),
            Level = managed.Level
        };
    }

    public static LoggerInfo ConvertToManaged(UnmanagedLoggerInfo unmanaged)
    {
        return new LoggerInfo()
        {
            Message = NativeStringViewMarshaller.ConvertToManaged(unmanaged.Message),
            Level = unmanaged.Level
        };
    }

    public static void Free(UnmanagedLoggerInfo unmanaged)
    {
        NativeStringViewMarshaller.Free(unmanaged.Message);
    }
}
