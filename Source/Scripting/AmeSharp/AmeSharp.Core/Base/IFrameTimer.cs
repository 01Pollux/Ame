using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("BE4B917D-4B7A-4582-8034-01DC24147418")]
public sealed class IFrameTimer : IBaseObject
{
    private IFrameTimer() : base(FrameTimerBridge.Create(), true, false) { }
    public static IFrameTimer Create() => new();

    public double EngineTime => FrameTimerBridge.GetEngineTime(this);
    public double GameTime => FrameTimerBridge.GetGameTime(this);
    public double DeltaTime => FrameTimerBridge.GetDeltaTime(this);

    public float TimeScale
    {
        get => FrameTimerBridge.GetTimeScale(this);
        set => FrameTimerBridge.SetTimeScale(this, value);
    }

    public bool IsPaused => FrameTimerBridge.IsPaused(this);

    public void Reset() => FrameTimerBridge.Reset(this);
    public void Tick() => FrameTimerBridge.Tick(this);
}
