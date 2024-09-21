using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Internal.Signals;

internal class FrameEnd_EventSignalImpl : ITypedSignalConnection<bool>
{
    public unsafe delegate nint InitializeCallback(delegate* unmanaged[Cdecl]<nint, nint, bool> callbackImpl);
    public readonly SignalCallback _callback;

    public unsafe FrameEnd_EventSignalImpl(SignalCallback callback, InitializeCallback initialize) : base(initialize(&SignalCallbackImpl))
    {
        _callback = callback;
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    private static bool SignalCallbackImpl(nint thisObject, nint _)
    {
        var @this = Get<FrameEnd_EventSignalImpl>(thisObject)!;
        return @this._callback();
    }
}
