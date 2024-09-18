using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;
using static AmeSharp.Core.Base.ISignalConnection;

namespace AmeSharp.Core.Base;

public class ISignal : INativeObject
{
    public ISignal() : base(SignalBridge.Create()) { }

    public ISignalConnection Connect(SignalCallback callback)
    {
        IntPtr connection;
        unsafe
        {
            var callbackPtr = (delegate* unmanaged[Cdecl]<void>)Marshal.GetFunctionPointerForDelegate(callback);
            connection = SignalBridge.Connect(NativePointer, callbackPtr, IntPtr.Zero);
        }
        return new ISignalConnection(connection, callback);
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
