using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

public class ISignalConnection : INativeObject
{
    private ISignalConnection(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static ISignalConnection Create(IntPtr handle) => new(handle, true);

    public void Disconnect() => Dispose();

    protected override bool ReleaseHandle()
    {
        SignalBridge.Disconnect(this);
        return true;
    }
}

public static class TypedSignalConnection<T> where T : unmanaged
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate T SignalCallback();
}

public static class VoidSignalConnection
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void SignalCallback();
}
