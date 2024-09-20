using AmeSharp.Bridge.Engine;
using AmeSharp.Core.Base;
using AmeSharp.Plugin;

namespace AmeSharp.Engine;

public class IAmeEngine : INativeObject
{
    public IAmeEngine() : base(AmeEngineBridge.Create())
    {
    }

    public void RefreshSubmoduleCache() => AmeEngineBridge.RefreshSubmoduleCache(NativePointer);

    public void Tick() => AmeEngineBridge.Tick(NativePointer);

    public bool IsRunning() => AmeEngineBridge.IsRunning(NativePointer);

    public void Exit(int exitCode) => AmeEngineBridge.Exit(NativePointer, exitCode);

    public IModuleRegistry Registry => new(AmeEngineBridge.GetModuleRegistry(NativePointer));

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != IntPtr.Zero)
        {
            AmeEngineBridge.Release(NativePointer);
        }
        base.Dispose(disposing);
    }
}
