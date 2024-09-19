using AmeSharp.Bridge.Core.Base;
using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Utils.Callbacks;

namespace AmeSharp.Core.Base;

public class ISignal : INativeObject
{
    public ISignal() : base(SignalBridge.Create()) { }

    public unsafe ISignalConnection Connect(CallbackUtils.Callback callback)
    {
        return new SignalConnectionImpl(callback, (callbackImpl, thisHandle) =>
        {
            return SignalBridge.Connect(NativePointer, callbackImpl, thisHandle);
        });
    }

    public void Emit() => SignalBridge.Emit(NativePointer, IntPtr.Zero);

    //

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != IntPtr.Zero)
        {
            SignalBridge.Release(NativePointer);
        }
        base.Dispose(disposing);
    }
}
