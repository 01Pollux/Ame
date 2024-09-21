﻿using AmeSharp.Bridge.Plugin;
using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Plugin;

public class IPlugin : INativeObject
{
    public IPlugin(IntPtr obj) : base(obj) { }
    public IPlugin(PluginInfo info) : base(PluginBridge.Create(info)) => InitialzeCallbacks();

    public PluginInfo Info => PluginBridge.GetInfo(NativePointer);
    public string Name => PluginBridge.GetName(NativePointer);
    public string Path => PluginBridge.GetPath(NativePointer);
    public bool Paused
    {
        get => PluginBridge.IsPaused(NativePointer);
        set => PluginBridge.Pause(NativePointer, value);
    }

    protected virtual bool OnPreLoad(IModuleRegistry registry)
    {
        return true;
    }

    protected virtual void OnLoad(IModuleRegistry registry)
    {
    }

    protected virtual void OnInterfaceDrop(IBaseObject iface)
    {
    }

    protected virtual void OnUnload()
    {
    }

    protected virtual void OnPauseChanged(bool paused)
    {
    }

    //

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static bool OnPreLoadCallback(IntPtr thisPointer, IntPtr registry)
    {
        var @this = Get<IPlugin>(thisPointer)!;
        var registryObject = Get<IModuleRegistry>(registry)!;
        return @this.OnPreLoad(registryObject);
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static void OnLoadCallback(IntPtr thisPointer, IntPtr registry)
    {
        var @this = Get<IPlugin>(thisPointer)!;
        var registryObject = Get<IModuleRegistry>(registry)!;
        @this.OnLoad(registryObject);
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static void OnInterfaceDropCallback(IntPtr thisPointer, IntPtr iface)
    {
        var @this = Get<IPlugin>(thisPointer)!;
        var ifaceObject = Get<IBaseObject>(iface)!;
        @this.OnInterfaceDrop(ifaceObject);
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static void OnUnloadCallback(IntPtr thisPointer)
    {
        var @this = Get<IPlugin>(thisPointer)!;
        @this.OnUnload();
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static void OnPauseChangedCallback(IntPtr thisPointer, bool paused)
    {
        var @this = Get<IPlugin>(thisPointer)!;
        @this.OnPauseChanged(paused);
    }

    private unsafe void InitialzeCallbacks()
    {
        PluginBridge.SetOnPreLoad(NativePointer, &OnPreLoadCallback);
        PluginBridge.SetOnLoad(NativePointer, &OnLoadCallback);
        PluginBridge.SetOnInterfaceDrop(NativePointer, &OnInterfaceDropCallback);
        PluginBridge.SetOnUnload(NativePointer, &OnUnloadCallback);
        PluginBridge.SetOnPauseChanged(NativePointer, &OnPauseChangedCallback);
    }
}
