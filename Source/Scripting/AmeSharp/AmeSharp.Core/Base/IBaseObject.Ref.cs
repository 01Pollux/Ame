using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

public abstract partial class IBaseObject
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal unsafe delegate IntPtr QueryInterfaceDelegate(IntPtr @object, Guid* iid);

    internal class BaseObjectRef(IntPtr handle) : IBaseObject(handle, false, false)
    {
    }

    private unsafe void InitializeCallbacks()
    {
        BaseObjectBridge.SetQueryInterface(Handle, (_, iid) =>
        {
            var result = OnQueryInterface(*iid);
            return result is not null ? result.Handle : InvalidHandle;
        });
    }
}
