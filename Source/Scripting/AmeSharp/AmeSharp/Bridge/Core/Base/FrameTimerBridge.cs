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
    public static partial double GetGameTime(IntPtr frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_GetEngineTime")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial double GetEngineTime(IntPtr frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_GetDeltaTime")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial double GetDeltaTime(IntPtr frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_GetTimeScale")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial float GetTimeScale(IntPtr frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_IsPaused")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsPaused(IntPtr frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_SetTimeScale")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetTimeScale(IntPtr frameTimer, float timeScale);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_Reset")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Reset(IntPtr frameTimer);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameTimer_Tick")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Tick(IntPtr frameTimer);
}
