using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

public class ISignal : INativeObject
{
    public delegate void SignalCallback();

    public ISignal() : base(SignalBridge.Create()) { }

    public ISignalConnection Connect(SignalCallback callback)
    {
        var callbackPtr = Marshal.GetFunctionPointerForDelegate(callback);
        var connection = SignalBridge.Connect(NativePointer, callbackPtr, IntPtr.Zero);
        return new ISignalConnection(connection);
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
