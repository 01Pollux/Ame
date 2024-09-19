using AmeSharp.Bridge.Core.Base;
using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Utils.Callbacks;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("919C726D-72E2-4573-ACC2-98EA5AF1781B")]
public class IFrameEvent : IBaseObject
{
    public IFrameEvent() : base(FrameEventBridge.Create()) { }

    public unsafe ISignalConnection ConnectOnFrameStart(CallbackUtils.Callback callback)
    {
        return new SignalConnectionImpl(callback, (callbackImpl, thisHandle) =>
        {
            return FrameEventBridge.ConnectOnFrameStart(NativePointer, callbackImpl, thisHandle);
        });
    }
    public unsafe ISignalConnection ConnectOnFrameUpdate(CallbackUtils.Callback callback)
    {
        return new SignalConnectionImpl(callback, (callbackImpl, thisHandle) =>
        {
            return FrameEventBridge.ConnectOnFrameUpdate(NativePointer, callbackImpl, thisHandle);
        });
    }
    public unsafe ISignalConnection ConnectOnFrameEnd(CallbackUtils.Callback callback)
    {
        return new SignalConnectionImpl(callback, (callbackImpl, thisHandle) =>
        {
            return FrameEventBridge.ConnectOnFrameEnd(NativePointer, callbackImpl, thisHandle);
        });
    }
}
