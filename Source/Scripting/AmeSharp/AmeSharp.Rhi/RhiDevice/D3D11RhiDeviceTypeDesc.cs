namespace AmeSharp.Rhi.RhiDevice;

public sealed record D3D11RhiDeviceTypeDesc : IRhiDeviceTypeDesc
{
    public Diligent.Version GraphicsAPIVersion = new(11, 0);
}
