using AmeSharp.Rhi.RhiDevice.Desc;
using static AmeSharp.Rhi.RhiDevice.Desc.VulkanRhiDeviceTypeDesc;

namespace AmeSharp.Rhi.Marshallers.RhiDevice;

internal static unsafe partial class RhiDeviceCreateDescMarshaller
{
    internal struct UnmanagedVulkanRhiDeviceTypeDesc()
    {
        public DescriptorPoolSizes MainDescriptorPool;
        public DescriptorPoolSizes DynamicDescriptorPool;

        public byte** RequiredLayerExtensions;
        public byte** RequiredInstanceExtensions;
        public byte** RequiredDeviceExtensions;

        public byte** IgnoredDebugMessageNames;

        public nint RequiredDeviceFeatures;

        public uint DeviceLocalMemoryPageSize;

        public uint HostVisibleMemoryPageSize;

        public uint DeviceLocalMemoryReserveSize;

        public uint HostVisibleMemoryReserveSize;

        public uint UploadHeapPageSize;

        public uint DynamicHeapSize;

        public uint DynamicHeapPageSize;

        public QueryHeapSizes QueryHeaps;

        public void Copy(VulkanRhiDeviceTypeDesc vulkan, StringPool pool)
        {
            MainDescriptorPool = vulkan.MainDescriptorPool;
            DynamicDescriptorPool = vulkan.DynamicDescriptorPool;
            RequiredDeviceFeatures = vulkan.RequiredDeviceFeatures;
            DeviceLocalMemoryPageSize = vulkan.DeviceLocalMemoryPageSize;
            HostVisibleMemoryPageSize = vulkan.HostVisibleMemoryPageSize;
            DeviceLocalMemoryReserveSize = vulkan.DeviceLocalMemoryReserveSize;
            HostVisibleMemoryReserveSize = vulkan.HostVisibleMemoryReserveSize;
            UploadHeapPageSize = vulkan.UploadHeapPageSize;
            DynamicHeapSize = vulkan.DynamicHeapSize;
            DynamicHeapPageSize = vulkan.DynamicHeapPageSize;
            QueryHeaps = vulkan.QueryHeaps;

            RequiredLayerExtensions = pool.Insert(vulkan.RequiredLayerExtensions);
            RequiredInstanceExtensions = pool.Insert(vulkan.RequiredInstanceExtensions);
            RequiredDeviceExtensions = pool.Insert(vulkan.RequiredDeviceExtensions);
            IgnoredDebugMessageNames = pool.Insert(vulkan.IgnoredDebugMessageNames);
        }
    }
}
