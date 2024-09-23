using AmeSharp.Rhi.Bridge.RhiDevice;
using AmeSharp.Rhi.Surface;
using Diligent;

namespace AmeSharp.Rhi.RhiDevice;

public partial class IRhiDevice
{
    private Lazy<IEngineFactory>? _factory;
    private Lazy<IRenderDevice>? _renderDevice;
    private Lazy<IDeviceContext>? _immediateContext;
    private IWindow? _window;
    private Lazy<ISwapChain?>? _swapchain;

    private void Initialize(IWindow? window = null)
    {
        _factory = new(() => new IEngineFactory(RhiDeviceBridge.GetFactory(NativePointer)));
        _renderDevice = new(() => new IRenderDevice(RhiDeviceBridge.GetRenderDevice(NativePointer)));
        _immediateContext = new(() => new IDeviceContext(RhiDeviceBridge.GetImmediateContext(NativePointer)));
        _window = window;
        _swapchain = new(() =>
        {
            var swapchain = RhiDeviceBridge.GetSwapchain(NativePointer);
            return swapchain == IntPtr.Zero ? null : new ISwapChain(swapchain);
        });
    }
}
