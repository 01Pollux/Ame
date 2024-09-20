using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;
using static AmeSharp.Core.Base.ISignalConnection;

namespace AmeSharp.Core.Base;

[Guid("919C726D-72E2-4573-ACC2-98EA5AF1781B")]
public class IFrameEvent : IBaseObject
{
    public IFrameEvent(IntPtr obj) : base(obj) { }
    public IFrameEvent() : base(FrameEventBridge.Create()) { }

    public unsafe ISignalConnection ConnectOnFrameStart(SignalCallback callback)
    {
        return new SignalConnectionImpl(callback, (callbackImpl, thisHandle) =>
        {
            return FrameEventBridge.ConnectOnFrameStart(NativePointer, callbackImpl, thisHandle);
        });
    }
    public unsafe ISignalConnection ConnectOnFrameUpdate(SignalCallback callback)
    {
        return new SignalConnectionImpl(callback, (callbackImpl, thisHandle) =>
        {
            return FrameEventBridge.ConnectOnFrameUpdate(NativePointer, callbackImpl, thisHandle);
        });
    }
    public unsafe ISignalConnection ConnectOnFrameEnd(SignalCallback callback)
    {
        return new SignalConnectionImpl(callback, (callbackImpl, thisHandle) =>
        {
            return FrameEventBridge.ConnectOnFrameEnd(NativePointer, callbackImpl, thisHandle);
        });
    }
}
