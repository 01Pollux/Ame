using AmeSharp.Bridge.Plugin;
using AmeSharp.Core.Base;
using AmeSharp.Core.Base.Types;

namespace AmeSharp.Core.Plugin;

public sealed class IModuleRegistry : INativeObject
{
    private IModuleRegistry(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static IModuleRegistry References(IntPtr handle) => new(handle, true);
    public static IModuleRegistry Create() => new(ModuleRegistryBridge.Create(), true);

    public VersionT Version => ModuleRegistryBridge.GetVersion(Handle);

    public void ExposeInterface(IPlugin? pluginOwner, Guid guid, IBaseObject iface)
    {
        ModuleRegistryBridge.ExposeInterface(Handle, pluginOwner is not null ? pluginOwner.Handle : InvalidHandle, guid, iface);
    }

    public void DropInterface(Guid guid)
    {
        ModuleRegistryBridge.DropInterface(Handle, guid);
    }

    public T? RequestInterface<T>(IPlugin? pluginOwner, Guid iid) where T : IBaseObject
    {
        return ModuleRegistryBridge.RequestInterface(Handle, pluginOwner is not null ? pluginOwner.Handle : InvalidHandle, iid) as T;
    }

    public T? RequestInterface<T>(IPlugin? pluginOwner) where T : IBaseObject
    {
        return RequestInterface<T>(pluginOwner, typeof(T).GUID);
    }

    public IPlugin? FindPlugin(string name)
    {
        return IPlugin.References(ModuleRegistryBridge.FindPlugin(Handle, name));
    }

    public IPlugin? BindPlugin(IPlugin pluginCaller, string name, bool isRequired)
    {
        return IPlugin.References(ModuleRegistryBridge.BindPlugin(Handle, pluginCaller.Handle, name, isRequired));
    }

    public IPlugin? LoadPlugin(string name)
    {
        return IPlugin.References(ModuleRegistryBridge.LoadPlugin(Handle, name));
    }

    public void UnloadPlugin(IPlugin plugin)
    {
        ModuleRegistryBridge.UnloadPlugin(Handle, plugin.Handle);
    }

    public void UnloadPlugin(string name)
    {
        ModuleRegistryBridge.UnloadPlugin(Handle, name);
    }

    protected override bool ReleaseHandle()
    {
        ModuleRegistryBridge.Release(handle);
        return true;
    }
}
