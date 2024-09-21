using AmeSharp.Bridge.Core.Base;
using AmeSharp.Bridge.Core.Internal.Signals;

namespace AmeSharp.Core.Base;

public class ISignal : INativeObject
{
    public ISignal() : base(SignalBridge.Create()) { }

    public unsafe ISignalConnection Connect(VoidSignalConnectionImpl.SignalCallback callback)
    {
        return new VoidSignalConnectionImpl(callback, (callbackImpl) =>
        {
            return SignalBridge.Connect(NativePointer, callbackImpl, IntPtr.Zero);
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
