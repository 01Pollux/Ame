using AmeSharp.Bridge.Core.Base;

namespace AmeSharp.Core.Base;

public abstract class ISignalConnection(IntPtr obj) : INativeObject(obj)
{
    public void Disconnect() => Dispose();

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != nint.Zero)
        {
            SignalBridge.Disconnect(NativePointer);
        }
        base.Dispose(disposing);
    }
}

public abstract class ITypedSignalConnection<T>(IntPtr obj) : ISignalConnection(obj)
{
    public delegate T SignalCallback();
}

public abstract class IVoidSignalConnection(IntPtr obj) : ISignalConnection(obj)
{
    public delegate void SignalCallback();
}
