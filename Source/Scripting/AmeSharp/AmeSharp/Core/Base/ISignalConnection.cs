using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;
using static AmeSharp.Core.Base.ISignalConnection;

namespace AmeSharp.Core.Base;

public class ISignalConnection(IntPtr obj, SignalCallback callback) : INativeObject(obj)
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void SignalCallback();
    private readonly SignalCallback _callback = callback;

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
