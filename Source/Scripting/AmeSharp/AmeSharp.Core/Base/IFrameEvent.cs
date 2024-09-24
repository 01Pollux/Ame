using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("919C726D-72E2-4573-ACC2-98EA5AF1781B")]
public sealed class IFrameEvent : IBaseObject
{
    private IFrameEvent() : base(FrameEventBridge.Create(), true, false) { }
    public static IFrameEvent Create() => new();

    public unsafe ISignalConnection ConnectOnFrameStart(VoidSignalConnection.SignalCallback callback)
    {
        return ISignalConnection.Create(FrameEventBridge.ConnectOnFrameStart(this, callback, nint.Zero));
    }
    public unsafe ISignalConnection ConnectOnFrameUpdate(VoidSignalConnection.SignalCallback callback)
    {
        return ISignalConnection.Create(FrameEventBridge.ConnectOnFrameUpdate(this, callback, nint.Zero));
    }
    public unsafe ISignalConnection ConnectOnFrameEnd(TypedSignalConnection<bool>.SignalCallback callback)
    {
        return ISignalConnection.Create(FrameEventBridge.ConnectOnFrameEnd(this, callback, nint.Zero));
    }
}
