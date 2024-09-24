using AmeSharp.Bridge.Core.Base;

namespace AmeSharp.Core.Base;

public class ISignal : INativeObject
{
    private ISignal(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static ISignal Create() => new(SignalBridge.Create(), true);

    public unsafe ISignalConnection Connect(VoidSignalConnection.SignalCallback callback)
    {
        return ISignalConnection.Create(SignalBridge.Connect(Handle, callback, nint.Zero));
    }

    public void Emit() => SignalBridge.Emit(Handle);

    //

    protected override bool ReleaseHandle()
    {
        SignalBridge.Release(Handle);
        return true;
    }
}
