#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Core.hpp>

namespace Ame::Window
{
    class IWindow;
} // namespace Ame::Window

namespace Ame::Rhi
{
    enum class DeviceValidationType : uint32_t
    {
        None = 0,

        /// Enable backend-specific validation (e.g. use Direct3D11 debug device, enable Direct3D12
        /// debug layer, enable Vulkan validation layers, create debug OpenGL context, etc.).
        /// The validation is enabled by default in Debug/Development builds and disabled
        /// in release builds.
        DebugLayer = 0x01,

        /// Verify that constant or structured buffer size is not smaller than what is expected by the shader.
        ///
        /// \remarks  This flag only has effect in Debug/Development builds.
        ///           This type of validation is never performed in Release builds.
        ///
        /// \note   This option is currently supported by Vulkan backend only.
        CheckShaderBufferSize = 0x01,

        /// D3D11:
        /// Verify that all committed context resources are relevant,
        /// i.e. they are consistent with the committed resource cache.
        /// This is very expensive and should only be used for engine debugging.
        /// This option is enabled in validation level 2 (see Diligent::VALIDATION_LEVEL).
        ///
        /// \remarks  This flag only has effect in Debug/Development builds.
        ///           This type of validation is never performed in Release builds.
        CommittedResourceRelevance = 1 << 0,

        /// D3D12:
        /// Whether to break execution when D3D12 debug layer detects an error.
        /// This flag only has effect if validation is enabled (EngineCreateInfo.EnableValidation is true).
        /// This option is disabled by default in all validation levels.
        BreakOnError = 1 << 1,

        /// D3D12:
        /// Whether to break execution when D3D12 debug layer detects a memory corruption.
        /// This flag only has effect if validation is enabled (EngineCreateInfo.EnableValidation is true).
        /// This option is enabled by default when validation is enabled.
        BreakOnCorruption = 1 << 2,

        /// D3D12:
        /// Enable validation on the GPU timeline.
        /// See https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-d3d12-debug-layer-gpu-based-validation
        /// This flag only has effect if validation is enabled (EngineCreateInfo.EnableValidation is true).
        /// This option is enabled in validation level 2 (see Diligent::VALIDATION_LEVEL).
        ///
        /// \note Enabling this option may slow things down a lot.
        GpuBasedValidation = 1 << 3,

        All = DebugLayer | CheckShaderBufferSize | CommittedResourceRelevance | BreakOnError | BreakOnCorruption |
              GpuBasedValidation
    };

    using GraphicsAdapterInfo       = Diligent::GraphicsAdapterInfo;
    using GraphicsAdapterInfoFinder = std::function<Opt<uint32_t>(std::span<const GraphicsAdapterInfo>)>;
    using DeviceFeatures            = Diligent::DeviceFeatures;

    //

    struct DeviceCreateDescD3D11
    {
        Dg::Version GraphicsAPIVersion{ 11, 0 };
    };

    struct DeviceCreateDescD3D12
    {
        Dg::Version GraphicsAPIVersion{ 12, 0 };

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
        struct
        {
            uint32_t MaxResources         = 0; // (CBV, SRV, UAV) 0 = Use whatever diligent default deems default.
            uint32_t MaxSamplers          = 0; // 0 = Use whatever diligent default deems default.
            uint32_t MaxRenderTargets     = 0; // 0 = Use whatever diligent default deems default.
            uint32_t MaxDepthStencilViews = 0; // 0 = Use whatever diligent default deems default.
        } CpuDescriptors;

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
        struct
        {
            uint32_t MaxResources = 0; // (CBV, SRV, UAV) 0 = Use whatever diligent default deems default.
            uint32_t MaxSamplers  = 0; // 0 = Use whatever diligent default deems default.
        } GpuDescriptors;

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
        struct
        {
            uint32_t MaxResources = 0; // (CBV, SRV, UAV) 0 = Use whatever diligent default deems default.
            uint32_t MaxSamplers  = 0; // 0 = Use whatever diligent default deems default.
        } GpuDynamicDescriptors;

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
        struct
        {
            uint32_t MaxResources = 0; // (CBV, SRV, UAV) 0 = Use whatever diligent default deems default.
            uint32_t MaxSamplers  = 0; // 0 = Use whatever diligent default deems default.
        } GpuDynamicDescriptorsChunk;

        /// <summary>
        /// A device context uses dynamic heap when it needs to allocate temporary
        /// CPU-accessible memory to update a resource via IDeviceContext::UpdateBuffer() or
        /// IDeviceContext::UpdateTexture(), or to map dynamic resources.
        /// Device contexts first request a chunk of memory from global dynamic
        /// resource manager and then suballocate from this chunk in a lock-free
        /// fashion. DynamicHeapPageSize defines the size of this chunk.
        /// </summary>
        uint32_t DynamicHeapPageSize = 0; // 0 = Use whatever diligent default deems default.

        /// <summary>
        /// Number of dynamic heap pages that will be reserved by the
        /// global dynamic heap manager to avoid page creation at run time.
        /// </summary>
        uint32_t NumDynamicHeapPagesToReserve = 0; // 0 = Use whatever diligent default deems default.

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
        struct
        {
            uint32_t Occlusion          = 0; // 0 = Use whatever diligent default deems default.
            uint32_t BinaryOcclusion    = 0; // 0 = Use whatever diligent default deems default.
            uint32_t Timestamp          = 0; // 0 = Use whatever diligent default deems default.
            uint32_t PipelineStatistics = 0; // 0 = Use whatever diligent default deems default.
            uint32_t Duration           = 0; // 0 = Use whatever diligent default deems default.
        } QueryHeapSizes;
    };

    struct DeviceCreateDescGL
    {
    };

    struct DeviceCreateDescVulkan
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
        struct
        {
            uint32_t MaxDescriptorSets                = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumSeparateSamplerDescriptors    = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumCombinedSamplerDescriptors    = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumSampledImageDescriptors       = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumStorageImageDescriptors       = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumUniformBufferDescriptors      = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumStorageBufferDescriptors      = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumUniformTexelBufferDescriptors = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumStorageTexelBufferDescriptors = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumInputAttachmentDescriptors    = 0; // 0 = Use whatever diligent default deems default.
            uint32_t NumAccelStructDescriptors        = 0; // 0 = Use whatever diligent default deems default.
        } MainDescriptorPool, DynamicDescriptorPool;

        std::span<const char*> RequiredLayerExtensions;
        std::span<const char*> RequiredInstanceExtensions;
        std::span<const char*> RequiredDeviceExtensions;

        std::span<const char*> IgnoredDebugMessageNames;

        void* RequiredDeviceFeatures = nullptr;

        /// <summary>
        /// Allocation granularity for device-local memory.
        ///
        /// \remarks    Device-local memory is used for USAGE_DEFAULT and USAGE_IMMUTABLE
        ///             GPU resources, such as buffers and textures.
        ///
        ///             If there is no available GPU memory, the resource will fail to be created.
        /// </summary>
        uint32_t DeviceLocalMemoryPageSize = 0; // 0 = Use whatever diligent default deems default.

        /// <summary>
        /// Allocation granularity for host-visible memory.
        ///
        /// \remarks   Host-visible memory is primarily used to upload data to GPU resources.
        /// </summary>
        uint32_t HostVisibleMemoryPageSize = 0; // 0 = Use whatever diligent default deems default.

        /// <summary>
        /// Amount of device-local memory reserved by the engine.
        /// The engine does not pre-allocate the memory, but rather keeps free
        /// pages when resources are released.
        /// </summary>
        uint32_t DeviceLocalMemoryReserveSize = 0; // 0 = Use whatever diligent default deems default.

        /// <summary>
        /// Amount of host-visible memory reserved by the engine.
        /// The engine does not pre-allocate the memory, but rather keeps free
        /// pages when resources are released.
        /// </summary>
        uint32_t HostVisibleMemoryReserveSize = 0; // 0 = Use whatever diligent default deems default.

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
        uint32_t UploadHeapPageSize = 0; // 0 = Use whatever diligent default deems default.

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
        uint32_t DynamicHeapSize = 0; // 0 = Use whatever diligent default deems default.

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
        uint32_t DynamicHeapPageSize = 0; // 0 = Use whatever diligent default deems default.

        /// <summary>
        /// \remarks    In Vulkan, queries are allocated from the pool, and
        ///             one pool may contain multiple queries of different types.
        ///             QueryPoolSizes array specifies the number of queries
        ///             of each type that will be allocated from a single pool.
        ///             The engine will create as many pools as necessary to
        ///             satisfy the requested number of queries.
        /// </summary>
        struct
        {
            uint32_t Occlusion          = 0; // 0 = Use whatever diligent default deems default.
            uint32_t BinaryOcclusion    = 0; // 0 = Use whatever diligent default deems default.
            uint32_t Timestamp          = 0; // 0 = Use whatever diligent default deems default.
            uint32_t PipelineStatistics = 0; // 0 = Use whatever diligent default deems default.
            uint32_t Duration           = 0; // 0 = Use whatever diligent default deems default.
        } QueryHeapSizes;
    };

    //

    using DeviceCreateDescVariant =
        std::variant<DeviceCreateDescD3D11, DeviceCreateDescD3D12, DeviceCreateDescGL, DeviceCreateDescVulkan>;

    using DeviceCreateDescStream = std::vector<DeviceCreateDescVariant>;

    //

    struct FullscreenModeDesc
    {
        struct
        {
            uint32_t Numerator   = 0;
            uint32_t Denominator = 0;
        } RefreshRate;

        Dg::SCALING_MODE   Scaling       = Dg::SCALING_MODE_UNSPECIFIED;
        Dg::SCANLINE_ORDER ScanlineOrder = Dg::SCANLINE_ORDER_UNSPECIFIED;
        bool               Fullscreen    = false;
    };

    struct SwapchainDesc
    {
        /// <summary>
        /// Back buffer format. Default value is Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB
        /// </summary>
        Dg::TEXTURE_FORMAT ColorFormat = Dg::TEX_FORMAT_RGBA8_UNORM_SRGB;

        /// <summary>
        /// Depth buffer format. Default value is Diligent::TEX_FORMAT_D32_FLOAT.
        /// Use Diligent::TEX_FORMAT_UNKNOWN to create the swap chain without depth buffer.
        /// </summary>
        Dg::TEXTURE_FORMAT DepthFormat = Dg::TEX_FORMAT_UNKNOWN;

        /// <summary>
        /// Swap chain usage flags. Default value is Diligent::SWAP_CHAIN_USAGE_RENDER_TARGET
        /// </summary>
        Dg::SWAP_CHAIN_USAGE_FLAGS Usage = Dg::SWAP_CHAIN_USAGE_RENDER_TARGET;

        /// <summary>
        /// The transform, relative to the presentation engine's natural orientation,
        /// applied to the image content prior to presentation.
        ///
        /// \note When default value (SURFACE_TRANSFORM_OPTIMAL) is used, the engine will
        ///       select the most optimal surface transformation. An application may request
        ///       specific transform (e.g. SURFACE_TRANSFORM_IDENTITY) and the engine will
        ///       try to use that. However, if the transform is not available, the engine will
        ///       select the most optimal transform.
        ///       After the swap chain has been created, this member will contain the actual
        ///       transform selected by the engine and can be queried through ISwapChain::GetDesc()
        ///       method.
        /// </summary>
        Dg::SURFACE_TRANSFORM PreTransform = Dg::SURFACE_TRANSFORM_OPTIMAL;

        /// <summary>
        /// The number of buffers in the swap chain
        /// </summary>
        uint32_t BufferCount = 0; // 0 = Use whatever diligent default deems default.

        /// <summary>
        /// Default depth value, which is used as optimized depth clear value in D3D12
        /// </summary>
        float DefaultDepthValue = 1.f;

        /// <summary>
        /// Default stencil value, which is used as optimized stencil clear value in D3D12
        /// </summary>
        uint8_t DefaultStencilValue = 0;

        /// <summary>
        /// Indicates if this is a primary swap chain. When Present() is called
        /// for the primary swap chain, the engine releases stale resources.
        /// </summary>
        bool IsPrimary = true;
    };

    struct RenderSurfaceDesc
    {
        Ptr<Window::IWindow> Window; // Window may be shared across multiple devices.
        FullscreenModeDesc   FullscreenMode;
        SwapchainDesc        Swapchain;
    };

    //

    struct DeviceCreateDesc
    {
        /// <summary>
        /// The device features to use.
        /// </summary>
        DeviceFeatures Features{ Dg::DEVICE_FEATURE_STATE_OPTIONAL };

        /// <summary>
        /// The adapter callback to use for the device.
        /// if not set, the first adapter will be used.
        /// </summary>
        GraphicsAdapterInfoFinder AdapterCallback = nullptr;

#ifdef AME_DEBUG_SANITIZE
        DeviceValidationType ValidationLayer = DeviceValidationType::All;
#elif defined(AME_DEBUG)
        DeviceValidationType ValidationLayer =
            static_cast<DeviceValidationType>(std::to_underlying(DeviceValidationType::BreakOnError) |
                                              std::to_underlying(DeviceValidationType::BreakOnCorruption));
#else
        DeviceValidationType ValidationLayer = DeviceValidationType::None;
#endif

        /// <summary>
        /// The types of devices to create
        /// The  first one that is supported will be created.
        /// </summary>
        DeviceCreateDescStream Types;

        /// <summary>
        /// A headless device is a device that does not create a window.
        /// </summary>
        Opt<RenderSurfaceDesc> Surface;

    public:
        [[nodiscard]] bool IsHeadless() const
        {
            return !Surface.has_value();
        }
    };
} // namespace Ame::Rhi