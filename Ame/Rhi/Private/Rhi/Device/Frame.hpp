#pragma once

#include <Rhi/Resource/CommandListImpl.hpp>
#include <Rhi/Device/DeferredResource.hpp>

#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    class MemoryAllocator;

    class Frame : public NonCopyable,
                  public NonMovable
    {
    public:
        /// <summary>
        /// Initializes the frame resource.
        /// </summary>
        void Initialize(
            nri::CoreInterface& NriCore,
            nri::CommandQueue&  GraphicsQueue,
            uint32_t            FrameIndex);

        /// <summary>
        /// Cleans up the frame resource.
        /// </summary>
        void Shutdown(
            nri::CoreInterface& NriCore);

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
            nri::CoreInterface& NriCore,
            MemoryAllocator&    MemAllocator);

        /// <summary>
        /// Ends the frame resource and submits the command buffer.
        /// </summary>
        void EndFrame(
            nri::CoreInterface& NriCore);

        /// <summary>
        /// Releases the resources that are deferred for release.
        /// </summary>
        void Release(
            nri::CoreInterface& NriCore,
            MemoryAllocator&    MemAllocator);

    public:
        /// <summary>
        /// Defer the release of a buffer.
        /// </summary>
        void DeferRelease(
            nri::Buffer& NriBuffer);

        /// <summary>
        /// Defer the release of a texture.
        /// </summary>
        void DeferRelease(
            nri::Texture& NriTexture);

        /// <summary>
        /// Defer the release of a descriptor.
        /// </summary>
        void DeferRelease(
            nri::Descriptor& NriDescriptor);

        /// <summary>
        /// Defer the release of a pipeline state.
        /// </summary>
        void DeferRelease(
            nri::Pipeline& Pipeline);

    private:
        CommandListImpl m_CommandList;

        DeferredBuffer     m_DeferredBuffers;
        DeferredTexture    m_DeferredTextures;
        DeferredDescriptor m_DeferredDescriptors;
        DeferredPipeline   m_DeferredPipelines;
    };
} // namespace Ame::Rhi