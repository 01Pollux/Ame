using AmeSharp.Core.Base;
using AmeSharp.Rhi.Bridge.RhiDevice;
using AmeSharp.Rhi.RhiDevice.Desc;
using AmeSharp.Rhi.Surface;
using Diligent;
using System.Runtime.InteropServices;

namespace AmeSharp.Rhi.RhiDevice;

[Guid("E5EF1DD2-C925-4B33-984D-E24143CB02E4")]
public partial class IRhiDevice : IBaseObject
{
    public IRhiDevice(IntPtr obj) : base(obj, true)
    {
        Initialize();
    }

    public IRhiDevice(RhiDeviceCreateDesc desc) : base(RhiDeviceBridge.Create(desc), true)
    {
        Initialize(desc.Surface?.Window);
    }

    public IEngineFactory Factory => _factory!.Value;
    public IRenderDevice RenderDevice => _renderDevice!.Value;
    public IDeviceContext ImmediateContext => _immediateContext!.Value;
    public IWindow? Window => _window;
    public ISwapChain? Swapchain => _swapchain!.Value;

    public RenderDeviceType Api => (RenderDeviceType)RhiDeviceBridge.GetGraphicsAPI(NativePointer);
    public string ApiName => RhiDeviceBridge.GetGraphicsAPIName(NativePointer);

    public bool BeginFrame()
    {
        return RhiDeviceBridge.BeginFrame(NativePointer);
    }

    public void AdvanceFrame(uint syncInterval = 0)
    {
        RhiDeviceBridge.AdvanceFrame(NativePointer, syncInterval);
    }

    protected override void Dispose(bool disposing)
    {
        if (disposing)
        {
            Window?.Dispose();
        }
        base.Dispose(disposing);
    }
}
