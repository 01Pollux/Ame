namespace AmeSharp.Rhi.Device;

public interface IRhiDeviceCreateDesc
{
}

public record RhiDeviceCreateDesc
{
    //public delegate int? AdapterFinderDelegate(GraphicsAdapterInfo info);

    //public DeviceFeatures DeviceFeatures;
    //public AdapterFinderDelegate? AdapterFinder;

#if DEBUG
    public RhiDeviceValidationType ValidationType = RhiDeviceValidationType.BreakOnCorruption | RhiDeviceValidationType.BreakOnError;
#else
    public RhiDeviceValidationType ValidationType = RhiDeviceValidationType.None;
#endif

    public IRhiDeviceCreateDesc[] CreateDescs = [];

    public RhiDeviceCreateDesc()
    {
        //DeviceFeatures = new()
        //{
        //    SeparablePrograms = DeviceFeatureState.Optional,
        //    ShaderResourceQueries = DeviceFeatureState.Optional,
        //    WireframeFill = DeviceFeatureState.Optional,
        //    MultithreadedResourceCreation = DeviceFeatureState.Optional,
        //    ComputeShaders = DeviceFeatureState.Optional,
        //    GeometryShaders = DeviceFeatureState.Optional,
        //    Tessellation = DeviceFeatureState.Optional,
        //    MeshShaders = DeviceFeatureState.Optional,
        //    RayTracing = DeviceFeatureState.Optional,
        //    BindlessResources = DeviceFeatureState.Optional,
        //    OcclusionQueries = DeviceFeatureState.Optional,
        //    BinaryOcclusionQueries = DeviceFeatureState.Optional,
        //    TimestampQueries = DeviceFeatureState.Optional,
        //    PipelineStatisticsQueries = DeviceFeatureState.Optional,
        //    DurationQueries = DeviceFeatureState.Optional,
        //    DepthBiasClamp = DeviceFeatureState.Optional,
        //    DepthClamp = DeviceFeatureState.Optional,
        //    IndependentBlend = DeviceFeatureState.Optional,
        //    DualSourceBlend = DeviceFeatureState.Optional,
        //    MultiViewport = DeviceFeatureState.Optional,
        //    TextureCompressionBC = DeviceFeatureState.Optional,
        //    VertexPipelineUAVWritesAndAtomics = DeviceFeatureState.Optional,
        //    PixelUAVWritesAndAtomics = DeviceFeatureState.Optional,
        //    TextureUAVExtendedFormats = DeviceFeatureState.Optional,
        //    ShaderFloat16 = DeviceFeatureState.Optional,
        //    ResourceBuffer16BitAccess = DeviceFeatureState.Optional,
        //    UniformBuffer16BitAccess = DeviceFeatureState.Optional,
        //    ShaderInputOutput16 = DeviceFeatureState.Optional,
        //    ShaderInt8 = DeviceFeatureState.Optional,
        //    ResourceBuffer8BitAccess = DeviceFeatureState.Optional,
        //    UniformBuffer8BitAccess = DeviceFeatureState.Optional,
        //    ShaderResourceStaticArrays = DeviceFeatureState.Optional,
        //    ShaderResourceRuntimeArrays = DeviceFeatureState.Optional,
        //    WaveOp = DeviceFeatureState.Optional,
        //    InstanceDataStepRate = DeviceFeatureState.Optional,
        //    NativeFence = DeviceFeatureState.Optional,
        //    TileShaders = DeviceFeatureState.Optional,
        //    TransferQueueTimestampQueries = DeviceFeatureState.Optional,
        //    VariableRateShading = DeviceFeatureState.Optional,
        //    SparseResources = DeviceFeatureState.Optional,
        //    SubpassFramebufferFetch = DeviceFeatureState.Optional,
        //    TextureComponentSwizzle = DeviceFeatureState.Optional,
        //    TextureSubresourceViews = DeviceFeatureState.Optional,
        //    NativeMultiDraw = DeviceFeatureState.Optional,
        //    AsyncShaderCompilation = DeviceFeatureState.Optional,
        //    FormattedBuffers = DeviceFeatureState.Optional,
        //};
    }
}
