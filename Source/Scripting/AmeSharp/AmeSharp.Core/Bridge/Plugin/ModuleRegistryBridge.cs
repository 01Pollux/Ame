using AmeSharp.Core.Base.Types;
using AmeSharp.Core.Bridge;
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
    public static partial void Release(IntPtr module);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_GetHostVersion")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial VersionT GetVersion(IntPtr module);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_ExposeInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void ExposeInterface(IntPtr module, IntPtr pluginOwner, Guid guid, IntPtr iface);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_DropInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void DropInterface(IntPtr module, Guid guid);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_RequestInterface")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr RequestInterface(IntPtr module, IntPtr pluginOwner, Guid guid);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_FindPlugin")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr FindPlugin(IntPtr module, NativeStringView name);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_BindPlugin")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr BindPlugin(IntPtr module, IntPtr pluginCaller, NativeStringView name, [MarshalAs(UnmanagedType.I1)] bool isRequired);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_LoadPlugin")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr LoadPlugin(IntPtr module, NativeStringView name);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_UnloadPlugin")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void UnloadPlugin(IntPtr module, IntPtr plugin);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_ModuleRegistry_UnloadPluginByName")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void UnloadPlugin(IntPtr module, NativeStringView name);
}
