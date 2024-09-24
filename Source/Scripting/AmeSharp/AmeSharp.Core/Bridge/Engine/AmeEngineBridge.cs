using AmeSharp.Core.Bridge;
using AmeSharp.Core.Engine;
using AmeSharp.Core.Plugin;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Engine;

internal partial class AmeEngineBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Engine_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Engine_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Release(IntPtr engine);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Engine_RefreshSubmoduleCache")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void RefreshSubmoduleCache(IAmeEngine engine);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Engine_Tick")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Tick(IAmeEngine engine);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Engine_IsRunning")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsRunning(IAmeEngine engine);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Engine_Exit")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Exit(IAmeEngine engine, int exitCode);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Engine_GetModuleRegistry")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetModuleRegistry(IAmeEngine engine);
}
