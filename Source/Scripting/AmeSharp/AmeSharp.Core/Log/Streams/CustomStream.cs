using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Log;
using AmeSharp.Core.Log.Marshallers;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Log.Streams;

public abstract class ICustomLoggerStream : ILoggerStream
{
    unsafe protected ICustomLoggerStream() : base(LoggerStreamBridge.CreateCallback(&StreamCallback, nint.Zero))
    {
    }

    public abstract void Write(LoggerInfo info);

    //

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static unsafe void StreamCallback(nint _thisPointer, LoggerInfoMarshaller.Unmanaged* unmanagedInfo, nint _)
    {
        var @this = Get<ICustomLoggerStream>(_thisPointer)!;
        var info = LoggerInfoMarshaller.ConvertToManaged(*unmanagedInfo);
        @this.Write(info);
    }
}
