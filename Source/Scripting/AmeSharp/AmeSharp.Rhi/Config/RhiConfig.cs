using AmeSharp.Core.Plugin;
using AmeSharp.Rhi.ImGui;
using AmeSharp.Rhi.RhiDevice;

namespace AmeSharp.Rhi.Config;

public record ImGuiModuleConfig
{
    const uint DefaultInitialVBSize = 1024;
    const uint DefaultInitialIBSize = 2048;

    public uint InitialVertexBufferSize = DefaultInitialVBSize;
    public uint InitialIndexBufferSize = DefaultInitialIBSize;

    public ImGuiColorConversionMode ConversionMode = ImGuiColorConversionMode.Auto;
}

public class RhiConfig
{
    public RhiDeviceCreateDesc DeviceDesc = new();
    public ImGuiModuleConfig? ImGuiConfig = new();

    public void RegisterInterface(IModuleRegistry registry, IPlugin? owner = null)
    {
    }
}
