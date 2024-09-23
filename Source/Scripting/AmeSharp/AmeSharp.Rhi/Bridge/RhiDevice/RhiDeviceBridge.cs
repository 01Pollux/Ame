using AmeSharp.Rhi.RhiDevice.Desc;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Rhi.Bridge.RhiDevice;

internal partial class RhiDeviceBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create(RhiDeviceCreateDesc createDesc);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_BeginFrame")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool BeginFrame(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_AdvanceFrame")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void AdvanceFrame(IntPtr rhiDevice, uint syncInterval);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetFactory")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetFactory(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetRenderDevice")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetRenderDevice(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetImmediateContext")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetImmediateContext(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetWindow")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetWindow(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetSwapchain")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetSwapchain(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetCommonRenderPass")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetCommonRenderPass(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetRenderStateCache")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetRenderStateCache(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetGraphicsAPI")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial uint GetGraphicsAPI(IntPtr rhiDevice);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IRhiDevice_GetGraphicsAPIName")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.LPStr)]
    public static partial string GetGraphicsAPIName(IntPtr rhiDevice);
}
