namespace AmeSharp.Core.Rhi.Device;

public sealed record VulkanRhiDeviceCreateDesc : IRhiDeviceCreateDesc
{
    /// <summary>
    /// MainDescriptorPool:
    /// Size of the main descriptor pool that is used to allocate descriptor sets
    /// for static and mutable variables. If allocation from the current pool fails,
    /// the engine creates another one.
    ///
    /// DynamicDescriptorPool:
    /// Size of the dynamic descriptor pool that is used to allocate descriptor sets
    /// for dynamic variables. Every device context has its own dynamic descriptor set allocator.
    /// The allocator requests pools from global dynamic descriptor pool manager, and then
    /// performs lock-free suballocations from the pool.
    /// </summary>
    public struct DescriptorPoolSizes()
    {
        public uint MaxDescriptorSets = 0; // 0 = Use whatever diligent default deems default.
        public uint NumSeparateSamplerDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumCombinedSamplerDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumSampledImageDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumStorageImageDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumUniformBufferDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumStorageBufferDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumUniformTexelBufferDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumStorageTexelBufferDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumInputAttachmentDescriptors = 0; // 0 = Use whatever diligent default deems default.
        public uint NumAccelStructDescriptors = 0; // 0 = Use whatever diligent default deems default.
    }
    public DescriptorPoolSizes MainDescriptorPool;
    public DescriptorPoolSizes DynamicDescriptorPool;

    public string[] RequiredLayerExtensions = [];
    public string[] RequiredInstanceExtensions = [];
    public string[] RequiredDeviceExtensions = [];

    public string[] IgnoredDebugMessageNames = [];

    public nint RequiredDeviceFeatures = nint.Zero;

    /// <summary>
    /// Allocation granularity for device-local memory.
    ///
    /// \remarks    Device-local memory is used for USAGE_DEFAULT and USAGE_IMMUTABLE
    ///             GPU resources, such as buffers and textures.
    ///
    ///             If there is no available GPU memory, the resource will fail to be created.
    /// </summary>
    public uint DeviceLocalMemoryPageSize = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Allocation granularity for host-visible memory.
    ///
    /// \remarks   Host-visible memory is primarily used to upload data to GPU resources.
    /// </summary>
    public uint HostVisibleMemoryPageSize = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Amount of device-local memory reserved by the engine.
    /// The engine does not pre-allocate the memory, but rather keeps free
    /// pages when resources are released.
    /// </summary>
    public uint DeviceLocalMemoryReserveSize = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Amount of host-visible memory reserved by the engine.
    /// The engine does not pre-allocate the memory, but rather keeps free
    /// pages when resources are released.
    /// </summary>
    public uint HostVisibleMemoryReserveSize = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Page size of the upload heap that is allocated by immediate/deferred
    /// contexts from the global memory manager to perform lock-free dynamic
    /// suballocations.
    /// Upload heap is used to update resources with IDeviceContext::UpdateBuffer()
    /// and IDeviceContext::UpdateTexture().
    ///
    /// \remarks    Upload pages are allocated in host-visible memory. When a
    ///             page becomes available, the engiene will keep it alive
    ///             if the total size of the host-visible memory is less than
    ///             HostVisibleMemoryReserveSize. Otherwise, the page will
    ///             be released.
    ///
    ///             On exit, the engine prints the number of pages that were
    ///             allocated by each context to the log, for example:
    ///
    ///                 Diligent Engine: Info: Upload heap of immediate context peak used/allocated frame
    ///                 size: 80.00 MB / 80.00 MB (80 pages)
    /// </summary>
    public uint UploadHeapPageSize = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Size of the dynamic heap (the buffer that is used to suballocate
    /// memory for dynamic resources) shared by all contexts.
    ///
    /// \remarks    The dynamic heap is used to allocate memory for dynamic
    ///             resources. Each time a dynamic buffer or dynamic texture is mapped,
    ///             the engine allocates a new chunk of memory from the dynamic heap.
    ///             At the end of the frame, all dynamic memory allocated for the frame
    ///             is recycled. However, it may not became available again until
    ///             all command buffers that reference the memory are executed by the GPU
    ///             (which typically happens 1-2 frames later). If space in the dynamic
    ///             heap is exhausted, the engine will wait for up to 60 ms for the
    ///             space released from previous frames to become available. If the space
    ///             is still not available, the engine will fail to map the resource
    ///             and return null pointer.
    ///             The dynamic heap is shared by all contexts and cannot be resized
    ///             on the fly. The application should track the amount of dynamic memory
    ///             it needs and set this variable accordingly. When the application exits,
    ///             the engine prints dynamic heap statistics to the log, for example:
    ///
    ///                 Diligent Engine: Info: Dynamic memory manager usage stats:
    ///                 Total size: 8.00 MB. Peak allocated size: 0.50 MB. Peak utilization: 6.2%
    ///
    ///             The peak allocated size (0.50 MB in the example above) is the value that
    ///             should be used to guide setting this variable. An application should always
    ///             allow some extra space in the dynamic heap to avoid running out of dynamic memory.
    /// </summary>
    public uint DynamicHeapSize = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Size of the memory chunk suballocated by immediate/deferred context from
    /// the global dynamic heap to perform lock-free dynamic suballocations.
    ///
    /// \remarks    Dynamic memory is not allocated directly from the dynamic heap.
    ///             Instead, when a context needs to allocate memory for a dynamic
    ///             resource, it allocates a chunk of memory from the global dynamic
    ///             heap (which requires synchronization with other contexts), and
    ///             then performs lock-free suballocations from the chunk.
    ///             The size of this chunk is set by DynamicHeapPageSize variable.
    ///
    ///             When the application exits, the engine prints dynamic heap statistics
    ///             for each context to the log, for example:
    ///
    ///                 Diligent Engine: Info: Dynamic heap of immediate context usage stats:
    ///                                        Peak used/aligned/allocated size: 94.14 KB / 94.56 KB / 256.00 KB (1
    ///                                        page). Peak efficiency (used/aligned): 99.6%. Peak utilization
    ///                                        (used/allocated): 36.8%
    ///
    ///             * Peak used size is the total amount of memory required for dynamic resources
    ///               allocated by the context during the frame.
    ///             * Peak aligned size is the total amount of memory required for dynamic resources
    ///               allocated by the context during the frame, accounting for necessary alignment. This
    ///               value is always greater than or equal to the peak used size.
    ///             * Peak allocated size is the total amount of memory allocated from the dynamic
    ///               heap by the context during the frame. This value is always a multiple of
    ///               DynamicHeapPageSize.
    /// </summary>
    public uint DynamicHeapPageSize = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// \remarks    In Vulkan, queries are allocated from the pool, and
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
