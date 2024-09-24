using AmeSharp.Bridge.Plugin;
using AmeSharp.Core.Base;

namespace AmeSharp.Core.Plugin;

public partial class IPlugin : INativeObject
{
    private IPlugin(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static IPlugin References(IntPtr handle) => new(handle, false);
    public IPlugin(PluginInfo info) : base(PluginBridge.Create(info), true)
    {
        InitialzeCallbacks();
    }

    public PluginInfo Info => PluginBridge.GetInfo(this);
    public string Name => PluginBridge.GetName(this);
    public string Path => PluginBridge.GetPath(this);
    public bool Paused
    {
        get => PluginBridge.IsPaused(this);
        set => PluginBridge.Pause(this, value);
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

    protected override bool ReleaseHandle()
    {
        PluginBridge.Release(Handle);
        return true;
    }
}
