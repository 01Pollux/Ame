using AmeSharp.Rhi.RhiDevice.Desc;
using static AmeSharp.Rhi.RhiDevice.Desc.D3D12RhiDeviceTypeDesc;

namespace AmeSharp.Rhi.Marshallers.RhiDevice;

internal static unsafe partial class RhiDeviceCreateDescMarshaller
{
    internal struct UnmanagedD3D12RhiDeviceTypeDesc()
    {
        public Diligent.Version GraphicsVersion;

        public CpuDescriptors CpuDesc;

        public GpuDescriptors GpuDesc;

        public GpuDynamicDescriptors GpuDynDesc;

        public GpuDynamicDescriptorsChunk GpuDynDescChunk;

        public uint DynamicHeapPageSize = 0;
        public uint NumDynamicHeapPagesToReserve = 0;

        public QueryHeapSizes QueryHeaps;

        public void Copy(D3D12RhiDeviceTypeDesc d3d12)
        {
            GraphicsVersion = d3d12.GraphicsAPIVersion;
            CpuDesc = d3d12.CpuDesc;
            GpuDesc = d3d12.GpuDesc;
            GpuDynDesc = d3d12.GpuDynDesc;
            GpuDynDescChunk = d3d12.GpuDynDescChunk;
            DynamicHeapPageSize = d3d12.DynamicHeapPageSize;
            NumDynamicHeapPagesToReserve = d3d12.NumDynamicHeapPagesToReserve;
            QueryHeaps = d3d12.QueryHeaps;
        }
    }
}
