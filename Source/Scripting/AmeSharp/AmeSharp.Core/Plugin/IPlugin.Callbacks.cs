using AmeSharp.Bridge.Plugin;
using AmeSharp.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Plugin;

public partial class IPlugin
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate bool PreLoadCallback(IntPtr _thisPointer, IntPtr registryPointer);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void LoadCallback(IntPtr _thisPointer, IntPtr registryPointer);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void InterfaceDropCallback(IntPtr _thisPointer, IntPtr ifacePointer);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void UnloadCallback(IntPtr _thisPointer);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void PauseChangedCallback(IntPtr _thisPointer, bool paused);

    private unsafe void InitialzeCallbacks()
    {
        PluginBridge.SetOnPreLoad(this, (_, registry) => OnPreLoad(IModuleRegistry.References(registry)));
        PluginBridge.SetOnLoad(this, (_, registry) => OnLoad(IModuleRegistry.References(registry)));
        PluginBridge.SetOnInterfaceDrop(this, (_, iface) => OnInterfaceDrop(IBaseObject.References(iface)));
        PluginBridge.SetOnUnload(this, (_) => OnUnload());
        PluginBridge.SetOnPauseChanged(this, (_, paused) => OnPauseChanged(paused));
    }
}
