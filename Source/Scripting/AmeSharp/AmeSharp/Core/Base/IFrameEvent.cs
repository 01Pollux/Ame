using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;
using static AmeSharp.Core.Base.ISignalConnection;

namespace AmeSharp.Core.Base;

[Guid("919C726D-72E2-4573-ACC2-98EA5AF1781B")]
public class IFrameEvent : IBaseObject
{
    public IFrameEvent() : base(FrameEventBridge.Create()) { }

    public ISignalConnection ConnectOnFrameStart(SignalCallback callback)
    {
        IntPtr connection;
        unsafe
        {
            var callbackPtr = (delegate* unmanaged[Cdecl]<void>)Marshal.GetFunctionPointerForDelegate(callback);
            connection = FrameEventBridge.ConnectOnFrameStart(NativePointer, callbackPtr, IntPtr.Zero);
        }
        return new(connection, callback);
    }
    public ISignalConnection ConnectOnFrameUpdate(SignalCallback callback)
    {
        IntPtr connection;
        unsafe
        {
            var callbackPtr = (delegate* unmanaged[Cdecl]<void>)Marshal.GetFunctionPointerForDelegate(callback);
            connection = FrameEventBridge.ConnectOnFrameUpdate(NativePointer, callbackPtr, IntPtr.Zero);
        }
        return new(connection, callback);
    }
    public ISignalConnection ConnectOnFrameEnd(SignalCallback callback)
    {
        IntPtr connection;
        unsafe
        {
            var callbackPtr = (delegate* unmanaged[Cdecl]<void>)Marshal.GetFunctionPointerForDelegate(callback);
            connection = FrameEventBridge.ConnectOnFrameEnd(NativePointer, callbackPtr, IntPtr.Zero);
        }
        return new(connection, callback);
    }
}
