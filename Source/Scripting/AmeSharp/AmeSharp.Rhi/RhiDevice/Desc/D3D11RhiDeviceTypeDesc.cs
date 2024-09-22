namespace AmeSharp.Rhi.RhiDevice.Desc;

public sealed record D3D11RhiDeviceTypeDesc : IRhiDeviceTypeDesc
{
    public Diligent.Version GraphicsAPIVersion = new(11, 0);
}
