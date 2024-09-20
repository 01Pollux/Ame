using AmeSharp.Bridge.Plugin;
using AmeSharp.Core.Base;
using AmeSharp.Core.Base.Types;

namespace AmeSharp.Plugin;

public class IModuleRegistry : INativeObject
{
    public IModuleRegistry(IntPtr obj) : base(obj) { }
    public IModuleRegistry() : base(ModuleRegistryBridge.Create()) { }

    public VersionT Version => ModuleRegistryBridge.GetVersion(NativePointer);

    public void ExposeInterface(IPlugin? pluginOwner, Guid guid, IBaseObject iface)
    {
        ModuleRegistryBridge.ExposeInterface(NativePointer, pluginOwner is not null ? pluginOwner.NativePointer : IntPtr.Zero, guid, iface.NativePointer);
    }

    public void DropInterface(Guid guid)
    {
        ModuleRegistryBridge.DropInterface(NativePointer, guid);
    }

    public T? RequestInterface<T>(IPlugin pluginOwner) where T : IBaseObject
    {
        var output = ModuleRegistryBridge.RequestInterface(NativePointer, pluginOwner.NativePointer, typeof(T).GUID);
        return IBaseObject.RequestInterface<T>(output);
    }

    public IPlugin? FindPlugin(string name)
    {
        var output = ModuleRegistryBridge.FindPlugin(NativePointer, new NativeStringView(name));
        return output == IntPtr.Zero ? null : new IPlugin(output);
    }

    public IPlugin? BindPlugin(IPlugin pluginCaller, string name, bool isRequired)
    {
        var output = ModuleRegistryBridge.BindPlugin(NativePointer, pluginCaller.NativePointer, new NativeStringView(name), isRequired);
        return output == IntPtr.Zero ? null : new IPlugin(output);
    }

    public IPlugin? LoadPlugin(IPlugin pluginCaller, string name, bool isRequired)
    {
        var output = ModuleRegistryBridge.LoadPlugin(NativePointer, pluginCaller.NativePointer, new NativeStringView(name), isRequired);
        return output == IntPtr.Zero ? null : new IPlugin(output);
    }

    public void UnloadPlugin(IPlugin plugin)
    {
        ModuleRegistryBridge.UnloadPlugin(NativePointer, plugin.NativePointer);
    }

    public void UnloadPlugin(string name)
    {
        ModuleRegistryBridge.UnloadPlugin(NativePointer, new NativeStringView(name));
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
