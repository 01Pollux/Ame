using AmeSharp.Core.Config;
using AmeSharp.Core.Plugin;
using AmeSharp.Rhi.ImGui;
using AmeSharp.Rhi.RhiDevice;
using AmeSharp.Rhi.RhiDevice.Desc;
using AmeSharp.Rhi.Surface;

namespace AmeSharp.Rhi.Config;

public record ImGuiModuleConfig
{
    const uint DefaultInitialVBSize = 1024;
    const uint DefaultInitialIBSize = 2048;

    public uint InitialVertexBufferSize = DefaultInitialVBSize;
    public uint InitialIndexBufferSize = DefaultInitialIBSize;

    public ImGuiColorConversionMode ConversionMode = ImGuiColorConversionMode.Auto;
}

public class RhiConfig : IModuleConfig
{
    public RhiDeviceCreateDesc DeviceDesc = new();
    public ImGuiModuleConfig? ImGuiConfig;

    public void ExposeInterface(IModuleRegistry registry, IPlugin? owner = null)
    {
        if (DeviceDesc.Surface?.Window is not null)
        {
            registry.ExposeInterface(owner, typeof(IWindow).GUID, DeviceDesc.Surface.Window);
        }

        using IRhiDevice rhiDevice = IRhiDevice.Create(DeviceDesc);
        registry.ExposeInterface(owner, typeof(IRhiDevice).GUID, rhiDevice);
    }
}
