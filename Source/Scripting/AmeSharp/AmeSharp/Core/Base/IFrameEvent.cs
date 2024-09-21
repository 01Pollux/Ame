using AmeSharp.Bridge.Core.Base;
using AmeSharp.Bridge.Core.Internal.Signals;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("919C726D-72E2-4573-ACC2-98EA5AF1781B")]
public sealed class IFrameEvent : IBaseObject
{
    public delegate bool FrameEndSignalCallback();

    public IFrameEvent(IntPtr obj) : base(obj, true) { }
    public IFrameEvent() : base(FrameEventBridge.Create(), false) { }

    public unsafe ISignalConnection ConnectOnFrameStart(IVoidSignalConnection.SignalCallback callback)
    {
        return new VoidSignalConnectionImpl(callback, (callbackImpl) =>
        {
            return FrameEventBridge.ConnectOnFrameStart(NativePointer, callbackImpl, IntPtr.Zero);
        });
    }
    public unsafe ISignalConnection ConnectOnFrameUpdate(IVoidSignalConnection.SignalCallback callback)
    {
        return new VoidSignalConnectionImpl(callback, (callbackImpl) =>
        {
            return FrameEventBridge.ConnectOnFrameUpdate(NativePointer, callbackImpl, IntPtr.Zero);
        });
    }
    public unsafe ISignalConnection ConnectOnFrameEnd(ITypedSignalConnection<bool>.SignalCallback callback)
    {
        return new FrameEnd_EventSignalImpl(callback, (callbackImpl) =>
        {
            return FrameEventBridge.ConnectOnFrameEnd(NativePointer, callbackImpl, IntPtr.Zero);
        });
    }
}
