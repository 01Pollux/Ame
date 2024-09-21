using AmeSharp.Core.Bridge;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Internal;

internal partial class AbstractStorageBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_AbstractStorage_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_AbstractStorage_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Release(IntPtr storage);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_AbstractStorage_Set")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Set(IntPtr storage, IntPtr key, IntPtr value);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_AbstractStorage_Get")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Get(IntPtr storage, IntPtr key);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_AbstractStorage_Remove")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Remove(IntPtr storage, IntPtr key);
}
