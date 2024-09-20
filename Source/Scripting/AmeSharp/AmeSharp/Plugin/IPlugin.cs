using AmeSharp.Bridge.Plugin;
using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Plugin;

public class IPlugin : INativeObject
{
    public IPlugin(IntPtr obj) : base(obj)
    {
        InitialzeCallbacks();
    }
    public IPlugin(PluginInfo info) : base(PluginBridge.Create(info))
    {
        InitialzeCallbacks();
    }

    public PluginInfo Info => PluginBridge.GetInfo(NativePointer);
    public string Name => PluginBridge.GetName(NativePointer);
    public string Path => PluginBridge.GetPath(NativePointer);
    public bool Paused
    {
        get => PluginBridge.IsPaused(NativePointer);
        set => PluginBridge.Pause(NativePointer, value);
    }

    private GCHandle _thisHandle;

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
    static bool OnPreLoadCallback(IntPtr thisObject, IntPtr registry)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IPlugin;
        var registryObject = new IModuleRegistry(registry);
        return @this!.OnPreLoad(registryObject);
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static void OnLoadCallback(IntPtr thisObject, IntPtr registry)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IPlugin;
        var registryObject = new IModuleRegistry(registry);
        @this!.OnLoad(registryObject);
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static void OnInterfaceDropCallback(IntPtr thisObject, IntPtr iface)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IPlugin;
        var ifaceObject = new IBaseObject(iface);
        @this!.OnInterfaceDrop(ifaceObject);
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static void OnUnloadCallback(IntPtr thisObject)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IPlugin;
        @this!.OnUnload();
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    static void OnPauseChangedCallback(IntPtr thisObject, bool paused)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IPlugin;
        @this!.OnPauseChanged(paused);
    }

    private unsafe void InitialzeCallbacks()
    {
        _thisHandle = GCHandle.Alloc(this, GCHandleType.Pinned);
        var thisPointer = GCHandle.ToIntPtr(_thisHandle);

        PluginBridge.SetOnPreLoad(thisPointer, &OnPreLoadCallback);
        PluginBridge.SetOnLoad(thisPointer, &OnLoadCallback);
        PluginBridge.SetOnInterfaceDrop(thisPointer, &OnInterfaceDropCallback);
        PluginBridge.SetOnUnload(thisPointer, &OnUnloadCallback);
        PluginBridge.SetOnPauseChanged(thisPointer, &OnPauseChangedCallback);
    }
}
