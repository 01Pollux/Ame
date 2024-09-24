using AmeSharp.Core.Base;
using AmeSharp.Core.Base.Types;
using AmeSharp.Core.Bridge;
using AmeSharp.Core.Plugin;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Plugin;

internal partial class PluginBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create(PluginInfo pluginInfo);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Release(IntPtr plugin);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_SetOnPreLoad")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial void SetOnPreLoad(IPlugin plugin, IPlugin.PreLoadCallback callback);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_SetOnLoad")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial void SetOnLoad(IPlugin plugin, IPlugin.LoadCallback callback);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_SetOnInterfaceDrop")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial void SetOnInterfaceDrop(IPlugin plugin, IPlugin.InterfaceDropCallback callback);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_SetOnUnload")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial void SetOnUnload(IPlugin plugin, IPlugin.UnloadCallback callback);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_SetOnPauseChanged")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial void SetOnPauseChanged(IPlugin plugin, IPlugin.PauseChangedCallback callback);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_GetPluginInfo")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial PluginInfo GetInfo(IPlugin plugin);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_GetPluginName")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial NativeStringView GetName(IPlugin plugin);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_GetPluginPath")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial NativeStringView GetPath(IPlugin plugin);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_IsPaused")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsPaused(IPlugin plugin);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Plugin_SetPauseState")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Pause(IPlugin plugin, [MarshalAs(UnmanagedType.I1)] bool pause);
}
