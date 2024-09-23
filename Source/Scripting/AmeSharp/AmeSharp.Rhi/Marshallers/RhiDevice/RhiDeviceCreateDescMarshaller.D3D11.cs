using AmeSharp.Rhi.RhiDevice.Desc;

namespace AmeSharp.Rhi.Marshallers.RhiDevice;

internal static unsafe partial class RhiDeviceCreateDescMarshaller
{
    internal struct UnmanagedD3D11RhiDeviceTypeDesc
    {
        public Diligent.Version GraphicsVersion;

        public void Copy(D3D11RhiDeviceTypeDesc d3d11)
        {
            GraphicsVersion = d3d11.GraphicsAPIVersion;
        }
    }
}
