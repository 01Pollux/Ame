#pragma once

#include <Core/Coroutine.hpp>
#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi
{
    class DeviceResourceAllocator
    {
    public:
        DeviceResourceAllocator(
            class DeviceImpl& rhiDeviceImpl,
            bool              ThreadSafe);

    public:
        /// <summary>
        /// Create a pipeline layout.
        /// </summary>
        [[nodiscard]] Co::result<PipelineLayout> CreatePipelineLayout(
            const PipelineLayoutDesc& desc);

    public:
        /// <summary>
        /// Destroy a pipeline layout.
        /// </summary>
        [[nodiscard]] Co::result<void> Release(
            PipelineLayout& pipelineLayout);

    public:
        /// <summary>
        /// Create a pipeline state object.
        /// </summary>
        [[nodiscard]] Co::result<PipelineState> CreatePipelineState(
            const GraphicsPipelineDesc& desc);

        /// <summary>
        /// Create a pipeline state object.
        /// </summary>
        [[nodiscard]] Co::result<PipelineState> CreatePipelineState(
            const ComputePipelineDesc& desc);

    public:
        /// <summary>
        /// Destroy a pipeline state object.
        /// </summary>
        [[nodiscard]] Co::result<void> Release(
            PipelineState& pipelineState);

    public:
        /// <summary>
        /// Create a buffer.
        /// </summary>
        [[nodiscard]] Co::result<Buffer> CreateBuffer(
            const BufferDesc& desc,
            MemoryLocation    location);

        /// <summary>
        /// Create a texture.
        /// </summary>
        [[nodiscard]] Co::result<Texture> CreateTexture(
            const TextureDesc& desc);

    public:
        /// <summary>
        /// Destroy a buffer.
        /// </summary>
        [[nodiscard]] Co::result<void> Release(
            Buffer& buffer);

        /// <summary>
        /// Destroy a texture.
        /// </summary>
        [[nodiscard]] Co::result<void> Release(
            Texture& texture);

    public:
        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] Co::result<ResourceView> CreateView(
            const Buffer&         buffer,
            const BufferViewDesc& desc);

        /// <summary>
        /// Create a texture view.
        /// </summary>
        [[nodiscard]] Co::result<ResourceView> CreateView(
            const Texture&         texture,
            const TextureViewDesc& desc);

        /// <summary>
        /// Create a sampler resource view.
        /// </summary>
        [[nodiscard]] Co::result<ResourceView> CreateSampler(
            const SamplerDesc& desc);

    public:
        /// <summary>
        /// Destroy a resource view.
        /// </summary>
        [[nodiscard]] Co::result<void> Release(
            ResourceView& view);

    private:
        /// <summary>
        /// Lock the resource allocator if it is thread-safe.
        /// </summary>
        [[nodiscard]] Co::result<Co::scoped_async_lock> TryLock();

    private:
        Ref<DeviceImpl> m_RhiDeviceImpl;

        Co::async_lock m_AsyncLock;
        bool           m_MultiThreaded = false;
    };
} // namespace Ame::Rhi