namespace AmeSharp.Rhi.RhiDevice;

public sealed record D3D12RhiDeviceTypeDesc : IRhiDeviceTypeDesc
{
    public Diligent.Version GraphicsAPIVersion = new(12, 0);

    /// <summary>
    /// The size of the GPU descriptor heap region designated to static/mutable
    /// shader resource variables.
    /// Every Shader Resource Binding object allocates one descriptor
    /// per any static/mutable shader resource variable (every array
    /// element counts) when the object is created. All required descriptors
    /// are allocated in one continuous chunk.
    /// GPUDescriptorHeapSize defines the total number of all descriptors
    /// that can be allocated across all SRB objects.
    /// Note that due to heap fragmentation, releasing two chunks of sizes
    /// N and M does not necessarily make the chunk of size N+M available.
    ///
    /// When the application exits, the engine prints the GPU descriptor heap
    /// statistics to the log, for example:
    ///
    ///     Diligent Engine: Info: D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER     GPU heap max allocated size
    ///     (static|dynamic): 0/128 (0.00%) | 0/1920 (0.00%). Diligent Engine: Info:
    ///     D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV GPU heap max allocated size (static|dynamic): 9/16384 (0.05%) |
    ///     128/32768 (0.39%).
    ///
    /// An application should monitor the GPU descriptor heap statistics and
    /// set GPUDescriptorHeapSize and GPUDescriptorHeapDynamicSize accordingly.
    /// </summary>
    public struct CpuDescriptors()
    {
        public uint MaxResources = 0; // (CBV, SRV, UAV) 0 = Use whatever diligent default deems default.
        public uint MaxSamplers = 0; // 0 = Use whatever diligent default deems default.
        public uint MaxRenderTargets = 0; // 0 = Use whatever diligent default deems default.
        public uint MaxDepthStencilViews = 0; // 0 = Use whatever diligent default deems default.
    };
    public CpuDescriptors CpuDesc;

    /// <summary>
    /// The size of the GPU descriptor heap region designated to static/mutable
    /// shader resource variables.
    /// Every Shader Resource Binding object allocates one descriptor
    /// per any static/mutable shader resource variable (every array
    /// element counts) when the object is created. All required descriptors
    /// are allocated in one continuous chunk.
    /// GPUDescriptorHeapSize defines the total number of all descriptors
    /// that can be allocated across all SRB objects.
    /// Note that due to heap fragmentation, releasing two chunks of sizes
    /// N and M does not necessarily make the chunk of size N+M available.
    ///
    /// When the application exits, the engine prints the GPU descriptor heap
    /// statistics to the log, for example:
    ///
    ///     Diligent Engine: Info: D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER     GPU heap max allocated size
    ///     (static|dynamic): 0/128 (0.00%) | 0/1920 (0.00%). Diligent Engine: Info:
    ///     D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV GPU heap max allocated size (static|dynamic): 9/16384 (0.05%) |
    ///     128/32768 (0.39%).
    ///
    /// An application should monitor the GPU descriptor heap statistics and
    /// set GPUDescriptorHeapSize and GPUDescriptorHeapDynamicSize accordingly.
    /// </summary>
    public struct GpuDescriptors()
    {
        public uint MaxResources = 0; // (CBV, SRV, UAV) 0 = Use whatever diligent default deems default.
        public uint MaxSamplers = 0; // 0 = Use whatever diligent default deems default.
    }
    public GpuDescriptors GpuDesc;

    /// <summary>
    /// The size of the GPU descriptor heap region designated to dynamic
    /// shader resource variables.
    /// Every Shader Resource Binding object allocates one descriptor
    /// per any dynamic shader resource variable (every array element counts)
    /// every time the object is committed via IDeviceContext::CommitShaderResources.
    /// All used dynamic descriptors are discarded at the end of the frame
    /// and recycled when they are no longer used by the GPU.
    /// GPUDescriptorHeapDynamicSize defines the total number of descriptors
    /// that can be used for dynamic variables across all SRBs and all frames
    /// currently in flight.
    /// Note that in Direct3D12, the size of sampler descriptor heap is limited
    /// by 2048. Since Diligent Engine allocates single heap for all variable types,
    /// GPUDescriptorHeapSize[1] + GPUDescriptorHeapDynamicSize[1] must not
    /// exceed 2048.
    /// </summary>
    public struct GpuDynamicDescriptors()
    {
        public uint MaxResources = 0; // (CBV, SRV, UAV) 0 = Use whatever diligent default deems default.
        public uint MaxSamplers = 0; // 0 = Use whatever diligent default deems default.
    }
    public GpuDynamicDescriptors GpuDynDesc;

    /// <summary>
    /// The size of the chunk that dynamic descriptor allocations manager
    /// requests from the main GPU descriptor heap.
    /// The total number of dynamic descriptors available across all frames in flight is
    /// defined by GPUDescriptorHeapDynamicSize. Every device context allocates dynamic
    /// descriptors in two stages: it first requests a chunk from the global heap, and the
    /// performs linear suballocations from this chunk in a lock-free manner. The size of
    /// this chunk is defined by DynamicDescriptorAllocationChunkSize, thus there will be total
    /// GPUDescriptorHeapDynamicSize/DynamicDescriptorAllocationChunkSize chunks in
    /// the heap of each type.
    /// </summary>
    public struct GpuDynamicDescriptorsChunk()
    {
        public uint MaxResources = 0; // (CBV, SRV, UAV) 0 = Use whatever diligent default deems default.
        public uint MaxSamplers = 0; // 0 = Use whatever diligent default deems default.
    }
    public GpuDynamicDescriptorsChunk GpuDynDescChunk;

    /// <summary>
    /// A device context uses dynamic heap when it needs to allocate temporary
    /// CPU-accessible memory to update a resource via IDeviceContext::UpdateBuffer() or
    /// IDeviceContext::UpdateTexture(), or to map dynamic resources.
    /// Device contexts first request a chunk of memory from global dynamic
    /// resource manager and then suballocate from this chunk in a lock-free
    /// fashion. DynamicHeapPageSize defines the size of this chunk.
    /// </summary>
    public uint DynamicHeapPageSize = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Number of dynamic heap pages that will be reserved by the
    /// global dynamic heap manager to avoid page creation at run time.
    /// </summary>
    public uint NumDynamicHeapPagesToReserve = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Query pool size for each query type.
    ///
    /// \remarks    In Direct3D12, queries are allocated from the pool, and
    ///             one pool may contain multiple queries of different types.
    ///             QueryPoolSizes array specifies the number of queries
    ///             of each type that will be allocated from a single pool.
    ///             The engine will create as many pools as necessary to
    ///             satisfy the requested number of queries.
    /// </summary>
    public struct QueryHeapSizes()
    {
        public uint Occlusion = 0; // 0 = Use whatever diligent default deems default.
        public uint BinaryOcclusion = 0; // 0 = Use whatever diligent default deems default.
        public uint Timestamp = 0; // 0 = Use whatever diligent default deems default.
        public uint PipelineStatistics = 0; // 0 = Use whatever diligent default deems default.
        public uint Duration = 0; // 0 = Use whatever diligent default deems default.
    }
    public QueryHeapSizes QueryHeaps;
}
