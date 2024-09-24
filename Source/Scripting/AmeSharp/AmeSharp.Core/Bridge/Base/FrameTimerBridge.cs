using AmeSharp.Core.Base;
using AmeSharp.Core.Bridge;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base;

internal partial class FrameTimerBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_GetGameTime")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial double GetGameTime(IFrameTimer frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_GetEngineTime")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial double GetEngineTime(IFrameTimer frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_GetDeltaTime")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial double GetDeltaTime(IFrameTimer frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_GetTimeScale")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial float GetTimeScale(IFrameTimer frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_IsPaused")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsPaused(IFrameTimer frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_SetTimeScale")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetTimeScale(IFrameTimer frameTimer, float timeScale);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_Reset")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Reset(IFrameTimer frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_Tick")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Tick(IFrameTimer frameTimer);
}
