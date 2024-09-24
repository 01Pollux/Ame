using AmeSharp.Core.Base;
using AmeSharp.Rhi.Bridge.RhiDevice;
using AmeSharp.Rhi.RhiDevice.Desc;
using AmeSharp.Rhi.Surface;
using Diligent;
using System.Runtime.InteropServices;

namespace AmeSharp.Rhi.RhiDevice;

[Guid("E5EF1DD2-C925-4B33-984D-E24143CB02E4")]
public sealed partial class IRhiDevice : IBaseObject
{
    private IRhiDevice(RhiDeviceCreateDesc desc) : base(RhiDeviceBridge.Create(desc), true, false)
    {
        Initialize(desc.Surface?.Window);
    }
    public static IRhiDevice Create(RhiDeviceCreateDesc desc) => new(desc);

    public IEngineFactory Factory => _factory!.Value;
    public IRenderDevice RenderDevice => _renderDevice!.Value;
    public IDeviceContext ImmediateContext => _immediateContext!.Value;
    public IWindow? Window => _window;
    public ISwapChain? Swapchain => _swapchain!.Value;

    public RenderDeviceType Api => (RenderDeviceType)RhiDeviceBridge.GetGraphicsAPI(this);
    public string ApiName => RhiDeviceBridge.GetGraphicsAPIName(this);

    public bool BeginFrame()
    {
        return RhiDeviceBridge.BeginFrame(this);
    }

    public void AdvanceFrame(uint syncInterval = 0)
    {
        RhiDeviceBridge.AdvanceFrame(this, syncInterval);
    }
}
