using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Internal.Signals;

internal class VoidSignalConnectionImpl : IVoidSignalConnection
{
    public unsafe delegate nint InitializeCallback(delegate* unmanaged[Cdecl]<nint, nint, void> callbackImpl);
    public readonly SignalCallback _callback;

    public unsafe VoidSignalConnectionImpl(SignalCallback callback, InitializeCallback initialize) : base(initialize(&SignalCallbackImpl))
    {
        _callback = callback;
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    private static void SignalCallbackImpl(nint thisObject, nint _)
    {
        var @this = Get<VoidSignalConnectionImpl>(thisObject)!;
        @this._callback();
    }
}
