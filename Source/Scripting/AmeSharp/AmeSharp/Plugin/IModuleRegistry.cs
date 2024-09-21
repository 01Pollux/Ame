using AmeSharp.Bridge.Plugin;
using AmeSharp.Core.Base;
using AmeSharp.Core.Base.Types;

namespace AmeSharp.Plugin;

public sealed class IModuleRegistry : INativeObject
{
    public IModuleRegistry(IntPtr obj) : base(obj) { }
    public IModuleRegistry() : base(ModuleRegistryBridge.Create()) { }

    public VersionT Version => ModuleRegistryBridge.GetVersion(NativePointer);

    public void ExposeInterface(IPlugin? pluginOwner, Guid guid, IBaseObject iface)
    {
        var pluginHandle = pluginOwner is not null ? pluginOwner.NativePointer : IntPtr.Zero;
        ModuleRegistryBridge.ExposeInterface(NativePointer, pluginHandle, guid, iface.NativePointer);
    }

    public void DropInterface(Guid guid)
    {
        ModuleRegistryBridge.DropInterface(NativePointer, guid);
    }

    public IBaseObject RequestInterface<T>(IPlugin? pluginOwner, Guid iid) where T : IBaseObject
    {
        var pluginHandle = pluginOwner is not null ? pluginOwner.NativePointer : IntPtr.Zero;
        return new IBaseObject(ModuleRegistryBridge.RequestInterface(NativePointer, pluginHandle, iid));
    }

    public T? RequestInterface<T>(IPlugin? pluginOwner) where T : IBaseObject
    {
        var pluginHandle = pluginOwner is not null ? pluginOwner.NativePointer : IntPtr.Zero;
        var output = ModuleRegistryBridge.RequestInterface(NativePointer, pluginHandle, typeof(T).GUID);
        return IBaseObject.RequestInterface<T>(output);
    }

    public IPlugin? FindPlugin(string name)
    {
        var output = ModuleRegistryBridge.FindPlugin(NativePointer, name);
        return output == IntPtr.Zero ? null : new IPlugin(output);
    }

    public IPlugin? BindPlugin(IPlugin pluginCaller, string name, bool isRequired)
    {
        var output = ModuleRegistryBridge.BindPlugin(NativePointer, pluginCaller.NativePointer, name, isRequired);
        return output == IntPtr.Zero ? null : new IPlugin(output);
    }

    public IPlugin? LoadPlugin(string name)
    {
        var output = ModuleRegistryBridge.LoadPlugin(NativePointer, name);
        return output == IntPtr.Zero ? null : new IPlugin(output);
    }

    public void UnloadPlugin(IPlugin plugin)
    {
        ModuleRegistryBridge.UnloadPlugin(NativePointer, plugin.NativePointer);
    }

    public void UnloadPlugin(string name)
    {
        ModuleRegistryBridge.UnloadPlugin(NativePointer, name);
    }

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != IntPtr.Zero)
        {
            ModuleRegistryBridge.Release(NativePointer);
        }
        base.Dispose(disposing);
    }
}
