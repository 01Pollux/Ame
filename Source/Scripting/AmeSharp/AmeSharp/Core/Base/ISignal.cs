using AmeSharp.Bridge.Core.Base;
using static AmeSharp.Core.Base.ISignalConnection;

namespace AmeSharp.Core.Base;

public class ISignal : INativeObject
{
    public ISignal() : base(SignalBridge.Create()) { }

    public unsafe ISignalConnection Connect(SignalCallback callback)
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
