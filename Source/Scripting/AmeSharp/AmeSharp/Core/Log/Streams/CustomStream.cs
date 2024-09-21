using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Log.Marshallers;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Log.Streams;

public abstract class ICustomStream : ILoggerStream
{
    private GCHandle _thisHandle;

    protected ICustomStream() : base(IntPtr.Zero)
    {
        _thisHandle = GCHandle.Alloc(this, GCHandleType.Pinned);
        unsafe
        {
            NativePointer = LoggerStreamBridge.CreateCallback(&StreamCallback, GCHandle.ToIntPtr(_thisHandle));
        }
    }

    public abstract void Write(LoggerInfo info);

    //

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static unsafe void StreamCallback(LoggerInfoMarshaller.Unmanaged* unmanagedInfo, IntPtr _thisHandle)
    {
        var @this = GCHandle.FromIntPtr(_thisHandle).Target as ICustomStream;
        var info = LoggerInfoMarshaller.ConvertToManaged(*unmanagedInfo);
        @this!.Write(info);
    }
}
