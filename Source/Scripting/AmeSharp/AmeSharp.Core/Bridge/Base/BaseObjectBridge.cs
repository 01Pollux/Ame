using AmeSharp.Core.Base;
using AmeSharp.Core.Bridge;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base;

internal partial class BaseObjectBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IBaseObject_AddRef")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial int AddRef(IntPtr @object);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IBaseObject_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial int Release(IntPtr @object);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IBaseObject_QueryInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr QueryInterface(IntPtr @object, Guid iid);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_BaseObject_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IBaseObject_SetQueryInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial void SetQueryInterface(IntPtr @object, IBaseObject.QueryInterfaceDelegate callback);
}
