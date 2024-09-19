using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base;

internal partial class BaseObjectBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IBaseObject_AddRef")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void AddRef(IntPtr thisObject);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IBaseObject_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Release(IntPtr thisObject);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IBaseObject_QueryInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr QueryInterface(IntPtr thisObject, ref Guid iid);
}
