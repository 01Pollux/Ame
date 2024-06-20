#pragma once

#include <Rhi/CommandList/CommandListImpl.hpp>
#include <Rhi/Device/DeferredResource.hpp>

#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    class MemoryAllocator;

    class Frame
    {
    public:
        Frame() = default;

        Frame(const Frame&) = delete;
        Frame(Frame&&)      = delete;

        Frame& operator=(const Frame&) = delete;
        Frame& operator=(Frame&&)      = delete;

        ~Frame() = default;

    public:
        /// <summary>
        /// Initializes the frame resource.
        /// </summary>
        void Initialize(
            DeviceImpl&                     rhiDevice,
            const DescriptorAllocationDesc& descriptorPoolDesc,
            uint32_t                        frameIndex);

        /// <summary>
        /// Cleans up the frame resource.
        /// </summary>
        void Shutdown();

    public:
        /// <summary>
        /// Get the command list.
        /// </summary>
        [[nodiscard]] CommandListImpl& GetCommandList() noexcept;

    public:
        /// <summary>
        /// Resets the frame resource and cleans up pending resources.
        /// </summary>
        void NewFrame(
            nri::CoreInterface& nriCore,
            MemoryAllocator&    memoryAllocator);

        /// <summary>
        /// Ends the frame resource and submits the command buffer.
        /// </summary>
        void EndFrame();

        /// <summary>
        /// Releases the resources that are deferred for release.
        /// </summary>
        void Release(
            nri::CoreInterface& nriCore,
            MemoryAllocator&    memoryAllocator);

    public:
        /// <summary>
        /// Defer the release of a buffer.
        /// </summary>
        void DeferRelease(
            nri::Buffer& nriBuffer);

        /// <summary>
        /// Defer the release of a texture.
        /// </summary>
        void DeferRelease(
            nri::Texture& nriTexture);

        /// <summary>
        /// Defer the release of a descriptor.
        /// </summary>
        void DeferRelease(
            nri::Descriptor& nriDescriptor);

        /// <summary>
        /// Defer the release of a pipeline state.
        /// </summary>
        void DeferRelease(
            nri::Pipeline& nriPipeline);

    private:
        CommandListImpl m_CommandList;

        DeferredBufferList     m_DeferredBuffers;
        DeferredTextureList    m_DeferredTextures;
        DeferredDescriptorList m_DeferredDescriptors;
        DeferredPipelineList   m_DeferredPipelines;
    };
} // namespace Ame::Rhi