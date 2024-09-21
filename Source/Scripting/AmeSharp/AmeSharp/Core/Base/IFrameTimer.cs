using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("BE4B917D-4B7A-4582-8034-01DC24147418")]
public sealed class IFrameTimer : IBaseObject
{
    public IFrameTimer(IntPtr obj) : base(obj, true) { }
    public IFrameTimer() : base(FrameTimerBridge.Create(), false) { }

    public double EngineTime => FrameTimerBridge.GetEngineTime(NativePointer);
    public double GameTime => FrameTimerBridge.GetGameTime(NativePointer);
    public double DeltaTime => FrameTimerBridge.GetDeltaTime(NativePointer);

    public float TimeScale
    {
        get => FrameTimerBridge.GetTimeScale(NativePointer);
        set => FrameTimerBridge.SetTimeScale(NativePointer, value);
    }

    public bool IsPaused => FrameTimerBridge.IsPaused(NativePointer);

    public void Reset() => FrameTimerBridge.Reset(NativePointer);
    public void Tick() => FrameTimerBridge.Tick(NativePointer);
}
