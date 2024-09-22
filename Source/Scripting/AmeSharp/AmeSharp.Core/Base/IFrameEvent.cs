using AmeSharp.Bridge.Core.Base;
using AmeSharp.Core.Bridge.Internal.Signals;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("919C726D-72E2-4573-ACC2-98EA5AF1781B")]
public sealed class IFrameEvent : IBaseObject
{
    public delegate bool FrameEndSignalCallback();

    public IFrameEvent(nint obj) : base(obj, true) { }
    public IFrameEvent() : base(FrameEventBridge.Create(), true) { }

    public unsafe ISignalConnection ConnectOnFrameStart(IVoidSignalConnection.SignalCallback callback)
    {
        return new VoidSignalConnectionImpl(callback, (callbackImpl) =>
        {
            return FrameEventBridge.ConnectOnFrameStart(NativePointer, callbackImpl, nint.Zero);
        });
    }
    public unsafe ISignalConnection ConnectOnFrameUpdate(IVoidSignalConnection.SignalCallback callback)
    {
        return new VoidSignalConnectionImpl(callback, (callbackImpl) =>
        {
            return FrameEventBridge.ConnectOnFrameUpdate(NativePointer, callbackImpl, nint.Zero);
        });
    }
    public unsafe ISignalConnection ConnectOnFrameEnd(ITypedSignalConnection<bool>.SignalCallback callback)
    {
        return new FrameEnd_EventSignalImpl(callback, (callbackImpl) =>
        {
            return FrameEventBridge.ConnectOnFrameEnd(NativePointer, callbackImpl, nint.Zero);
        });
    }
}
