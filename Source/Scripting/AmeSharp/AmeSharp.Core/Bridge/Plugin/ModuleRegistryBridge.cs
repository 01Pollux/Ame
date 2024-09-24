using AmeSharp.Core.Base;
using AmeSharp.Core.Base.Types;
using AmeSharp.Core.Bridge;
using AmeSharp.Core.Plugin;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Plugin;

internal partial class ModuleRegistryBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Release(IntPtr registry);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_GetHostVersion")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial VersionT GetVersion(IntPtr registry);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_ExposeInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void ExposeInterface(IntPtr registry, IntPtr pluginOwner, Guid guid, IBaseObject iface);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_DropInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void DropInterface(IntPtr registry, Guid guid);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_RequestInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr RequestInterface(IntPtr registry, IntPtr pluginOwner, Guid guid);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_FindPlugin")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr FindPlugin(IntPtr registry, NativeStringView name);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_BindPlugin")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr BindPlugin(IntPtr registry, IntPtr pluginCaller, NativeStringView name, [MarshalAs(UnmanagedType.I1)] bool isRequired);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_LoadPlugin")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr LoadPlugin(IntPtr registry, NativeStringView name);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_UnloadPlugin")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void UnloadPlugin(IntPtr registry, IntPtr plugin);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_UnloadPluginByName")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void UnloadPlugin(IntPtr registry, NativeStringView name);
}
