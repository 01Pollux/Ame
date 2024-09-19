using AmeSharp.Core.Base;
using AmeSharp.Core.Utils.Callbacks;

namespace AmeSharp.Bridge.Core.Base;

public class SignalConnectionImpl : ISignalConnection
{
    private readonly CallbackHandler _callbackHandler;

    public SignalConnectionImpl(CallbackUtils.Callback callback, CallbackUtils.CallbackRegister initCallback) : base(nint.Zero)
    {
        _callbackHandler = new(callback, initCallback);
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
}
