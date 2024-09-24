using AmeSharp.Rhi.Bridge.RhiDevice;
using AmeSharp.Rhi.Surface;
using Diligent;

namespace AmeSharp.Rhi.RhiDevice;

public sealed partial class IRhiDevice
{
    private Lazy<IEngineFactory>? _factory;
    private Lazy<IRenderDevice>? _renderDevice;
    private Lazy<IDeviceContext>? _immediateContext;
    private IWindow? _window;
    private Lazy<ISwapChain?>? _swapchain;

    private void Initialize(IWindow? window = null)
    {
        _factory = new(() => new IEngineFactory(RhiDeviceBridge.GetFactory(this)));
        _renderDevice = new(() => new IRenderDevice(RhiDeviceBridge.GetRenderDevice(this)));
        _immediateContext = new(() => new IDeviceContext(RhiDeviceBridge.GetImmediateContext(this)));
        _window = window;
        _swapchain = new(() =>
        {
            var swapchain = RhiDeviceBridge.GetSwapchain(this);
            return swapchain == IntPtr.Zero ? null : new ISwapChain(swapchain);
        });
    }

    protected override void Dispose(bool disposing)
    {
        void DisposeIfCreated<T>(Lazy<T>? lazy) where T : IDisposable
        {
            if (lazy?.IsValueCreated == true)
            {
                lazy.Value.Dispose();
            }
        }

        void DisposeIfCreatedMaybeNull<T>(Lazy<T?>? lazy) where T : IDisposable
        {
            if (lazy?.IsValueCreated == true)
            {
                lazy.Value?.Dispose();
            }
        }

        if (disposing)
        {
            DisposeIfCreated(_factory);
            DisposeIfCreated(_renderDevice);
            DisposeIfCreated(_immediateContext);
            DisposeIfCreatedMaybeNull(_swapchain);
        }
        base.Dispose(disposing);
    }
}
