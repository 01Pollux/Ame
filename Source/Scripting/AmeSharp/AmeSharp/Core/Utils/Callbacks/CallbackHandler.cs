
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Utils.Callbacks;

public class CallbackHandler
{
    public IntPtr NativePointer { get; private set; }
    private GCHandle _thisHandle;
    private CallbackUtils.Callback _callback;

    public CallbackHandler(CallbackUtils.Callback callback, CallbackUtils.CallbackRegister initCallback)
    {
        _callback = callback;
        _thisHandle = GCHandle.Alloc(this, GCHandleType.Pinned);
        unsafe
        {
            delegate* unmanaged[Cdecl]<nint, void> implCallback = &SignalCallbackImpl;
            NativePointer = initCallback(implCallback, GCHandle.ToIntPtr(_thisHandle));
        }
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    private static unsafe void SignalCallbackImpl(nint _thisHandle)
    {
        var @this = GCHandle.FromIntPtr(_thisHandle).Target as CallbackHandler;
        @this!._callback();
    }
}
