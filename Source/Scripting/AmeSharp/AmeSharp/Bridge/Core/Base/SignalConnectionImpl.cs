using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base;

public class SignalConnectionImpl : ISignalConnection
{
    public unsafe delegate nint InitializeCallback(delegate* unmanaged[Cdecl]<IntPtr, void> callbackImpl, nint thisHandle);

    private GCHandle _thisHandle;
    private SignalCallback _callback;

    public SignalConnectionImpl(SignalCallback callback, InitializeCallback initialize) : base(nint.Zero)
    {
        _callback = callback;
        _thisHandle = GCHandle.Alloc(this, GCHandleType.Pinned);
        unsafe
        {
            initialize(&SignalCallbackImpl, GCHandle.ToIntPtr(_thisHandle));
        }
    }

    public override void Disconnect() => Dispose();

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != nint.Zero)
        {
            SignalBridge.Disconnect(NativePointer);
        }
        base.Dispose(disposing);
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    private static void SignalCallbackImpl(IntPtr thisObject)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as SignalConnectionImpl;
        @this!._callback();
    }
}
