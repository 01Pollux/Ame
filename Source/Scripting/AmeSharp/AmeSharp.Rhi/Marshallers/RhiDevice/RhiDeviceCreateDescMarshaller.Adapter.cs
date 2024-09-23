using Diligent;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Rhi.Marshallers.RhiDevice;

internal static unsafe partial class RhiDeviceCreateDescMarshaller
{
    [StructLayout(LayoutKind.Sequential)]
    internal unsafe struct UnmanagedCommandQueueInfo
    {
        public CommandQueueType QueueType;
        public uint MaxDeviceContexts;
        public uint _TextureCopyGranularity0;
        public uint _TextureCopyGranularity1;
        public uint _TextureCopyGranularity2;

        public CommandQueueInfo ToManaged()
        {
            return new()
            {
                QueueType = QueueType,
                MaxDeviceContexts = MaxDeviceContexts,
                TextureCopyGranularity = [_TextureCopyGranularity0, _TextureCopyGranularity1, _TextureCopyGranularity2]
            };
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    internal unsafe struct UnmanagedGraphicsAdapterInfo
    {
        public fixed byte Description[128];
        public AdapterType Type;
        public AdapterVendor Vendor;
        public uint VendorId;
        public uint DeviceId;
        public uint NumOutputs;
        public AdapterMemoryInfo Memory;
        public RayTracingProperties RayTracing;
        public WaveOpProperties WaveOp;
        public BufferProperties Buffer;
        public TextureProperties Texture;
        public SamplerProperties Sampler;
        public MeshShaderProperties MeshShader;
        public ComputeShaderProperties ComputeShader;
        public DrawCommandProperties DrawCommand;
        public SparseResourceProperties SparseResources;
        public DeviceFeatures Features;
        public UnmanagedCommandQueueInfo _Queue0;
        public UnmanagedCommandQueueInfo _Queue1;
        public UnmanagedCommandQueueInfo _Queue2;
        public UnmanagedCommandQueueInfo _Queue3;
        public UnmanagedCommandQueueInfo _Queue4;
        public UnmanagedCommandQueueInfo _Queue5;
        public UnmanagedCommandQueueInfo _Queue6;
        public UnmanagedCommandQueueInfo _Queue7;
        public UnmanagedCommandQueueInfo _Queue8;
        public UnmanagedCommandQueueInfo _Queue9;
        public UnmanagedCommandQueueInfo _Queue10;
        public UnmanagedCommandQueueInfo _Queue11;
        public UnmanagedCommandQueueInfo _Queue12;
        public UnmanagedCommandQueueInfo _Queue13;
        public UnmanagedCommandQueueInfo _Queue14;
        public UnmanagedCommandQueueInfo _Queue15;
        public uint NumQueues;

        public GraphicsAdapterInfo ToManaged()
        {
            string descriptionStr;
            fixed (byte* descriptionPtr = Description)
            {
                descriptionStr = Utf8StringMarshaller.ConvertToManaged(descriptionPtr) ?? string.Empty;
            }

            CommandQueueInfo[] queues = [
                _Queue0.ToManaged(), _Queue1.ToManaged(), _Queue2.ToManaged(), _Queue3.ToManaged(),
                _Queue4.ToManaged(), _Queue5.ToManaged(), _Queue6.ToManaged(), _Queue7.ToManaged(),
                _Queue8.ToManaged(), _Queue9.ToManaged(), _Queue10.ToManaged(), _Queue11.ToManaged(),
                _Queue12.ToManaged(), _Queue13.ToManaged(), _Queue14.ToManaged(), _Queue15.ToManaged(),
            ];
            return new()
            {
                Description = descriptionStr,
                Type = Type,
                Vendor = Vendor,
                VendorId = VendorId,
                DeviceId = DeviceId,
                NumOutputs = NumOutputs,
                Memory = Memory,
                RayTracing = RayTracing,
                WaveOp = WaveOp,
                Buffer = Buffer,
                Texture = Texture,
                Sampler = Sampler,
                MeshShader = MeshShader,
                ComputeShader = ComputeShader,
                DrawCommand = DrawCommand,
                SparseResources = SparseResources,
                Features = Features,
                Queues = queues,
                NumQueues = NumQueues
            };
        }
    }
}
