using AmeSharp.Bridge.Core.Base;

namespace AmeSharp.Core.Base;

public class ISignalConnection(IntPtr obj) : INativeObject(obj)
{
    public void Disconnect() => Dispose(true);

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != IntPtr.Zero)
        {
            SignalBridge.Disconnect(NativePointer);
        }
        base.Dispose(disposing);
    }
}
