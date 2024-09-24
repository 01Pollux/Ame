using AmeSharp.Bridge.Engine;
using AmeSharp.Core.Base;
using AmeSharp.Core.Plugin;

namespace AmeSharp.Core.Engine;

public sealed class IAmeEngine : INativeObject
{
    private IAmeEngine(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static IAmeEngine Create() => new(AmeEngineBridge.Create(), true);

    public void RefreshSubmoduleCache() => AmeEngineBridge.RefreshSubmoduleCache(this);

    public void Tick() => AmeEngineBridge.Tick(this);

    public bool IsRunning() => AmeEngineBridge.IsRunning(this);

    public void Exit(int exitCode) => AmeEngineBridge.Exit(this, exitCode);

    public IModuleRegistry Registry => IModuleRegistry.References(AmeEngineBridge.GetModuleRegistry(this));

    protected override bool ReleaseHandle()
    {
        AmeEngineBridge.Release(handle);
        return true;
    }
}
