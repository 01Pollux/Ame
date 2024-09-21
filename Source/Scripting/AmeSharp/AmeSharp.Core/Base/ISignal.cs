using AmeSharp.Bridge.Core.Base;
using AmeSharp.Core.Bridge.Internal.Signals;

namespace AmeSharp.Core.Base;

public class ISignal : INativeObject
{
    public ISignal() : base(SignalBridge.Create()) { }

    public unsafe ISignalConnection Connect(IVoidSignalConnection.SignalCallback callback)
    {
        return new VoidSignalConnectionImpl(callback, (callbackImpl) =>
        {
            return SignalBridge.Connect(NativePointer, callbackImpl, nint.Zero);
        });
    }

    public void Emit() => SignalBridge.Emit(NativePointer, nint.Zero);

    //

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != nint.Zero)
        {
            SignalBridge.Release(NativePointer);
        }
        base.Dispose(disposing);
    }
}
