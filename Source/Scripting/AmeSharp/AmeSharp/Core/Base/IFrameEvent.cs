using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("919C726D-72E2-4573-ACC2-98EA5AF1781B")]
public class IFrameEvent : IBaseObject
{
    public delegate void FrameEventCallback();

    public IFrameEvent() : base(FrameEventBridge.Create()) { }

    public ISignalConnection ConnectOnFrameStart(FrameEventCallback callback)
    {
        var callbackPtr = Marshal.GetFunctionPointerForDelegate(callback);
        return new(FrameEventBridge.ConnectOnFrameStart(NativePointer, callbackPtr, IntPtr.Zero));
    }
    public ISignalConnection ConnectOnFrameUpdate(FrameEventCallback callback)
    {
        var callbackPtr = Marshal.GetFunctionPointerForDelegate(callback);
        return new(FrameEventBridge.ConnectOnFrameUpdate(NativePointer, callbackPtr, IntPtr.Zero));
    }
    public ISignalConnection ConnectOnFrameEnd(FrameEventCallback callback)
    {
        var callbackPtr = Marshal.GetFunctionPointerForDelegate(callback);
        return new(FrameEventBridge.ConnectOnFrameEnd(NativePointer, callbackPtr, IntPtr.Zero));
    }
}
