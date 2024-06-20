#pragma once

#include <Rhi/CommandList/SubmissionContext.hpp>
#include <Rhi/Device/DeferredResource.hpp>
#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    class Frame
    {
    public:
        /// <summary>
        /// Shuts down the frame resource.
        /// </summary>
        void Shutdown(
            nri::CoreInterface& nriCore);

    public:
        /// <summary>
        /// Allocates a submission context for the given command queue.
        /// </summary>
        [[nodiscard]] SubmissionContext AllocateSubmission(
            nri::CoreInterface& nriCore,
            nri::Device&        nriDevice,
            nri::CommandQueue&  nriCommandQueue,
            CommandQueueType    queueType,
            StageBits           stages);

    public:
        /// <summary>
        /// Resets the frame resource and cleans up pending resources.
        /// </summary>
        void NewFrame(
            nri::CoreInterface&     nriCore,
            IDeviceMemoryAllocator& memoryAllocator);

        /// <summary>
        /// Releases the resources that are deferred for release.
        /// </summary>
        void Release(
            nri::CoreInterface&     nriCore,
            IDeviceMemoryAllocator& memoryAllocator);

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
        struct CommandInfo
        {
            nri::CommandAllocator* NriCommandAllocator = nullptr;
            nri::CommandBuffer*    NriCommandBuffer    = nullptr;
            nri::Fence*            NriFence            = nullptr;
            uint64_t               FenceValue          = 0;

            CommandInfo(
                nri::CoreInterface& nriCore,
                nri::Device&        nriDevice,
                nri::CommandQueue&  nriCommandQueue,
                CommandQueueType    queueType);
        };

        struct CommandInfoPool
        {
            std::vector<CommandInfo> FreeCommands;
            std::vector<CommandInfo> PendingCommands;
        };

        using CommandInfoPools = std::array<CommandInfoPool, static_cast<size_t>(CommandQueueType::MAX_NUM)>;

    private:
        CommandInfoPools m_CommandInfoPools;

        DeferredBufferList     m_DeferredBuffers;
        DeferredTextureList    m_DeferredTextures;
        DeferredDescriptorList m_DeferredDescriptors;
        DeferredPipelineList   m_DeferredPipelines;
    };
} // namespace Ame::Rhi