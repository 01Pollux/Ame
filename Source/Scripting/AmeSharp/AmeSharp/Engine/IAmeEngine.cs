using AmeSharp.Bridge.Engine;
using AmeSharp.Core.Base;

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

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != IntPtr.Zero)
        {
            AmeEngineBridge.Release(NativePointer);
        }
        base.Dispose(disposing);
    }
}
