#pragma once

#include <Rhi/Staging/StagedTexture.hpp>
#include <Rhi/Staging/StagedBuffer.hpp>

namespace Ame::Rhi::Staging
{
    class DeferredStagingManager
    {
    public:
        DeferredStagingManager(
            Device& rhiDevice);

        DeferredStagingManager(const DeferredStagingManager&) = delete;
        DeferredStagingManager(DeferredStagingManager&&)      = delete;

        DeferredStagingManager& operator=(const DeferredStagingManager&) = delete;
        DeferredStagingManager& operator=(DeferredStagingManager&&)      = delete;

        ~DeferredStagingManager();

    public:
        /// <summary>
        /// Queue upload buffer to buffer
        /// </summary>
        void QueueBufferCopy(
            const DeferredBufferCopyDesc& src,
            const DeferredBufferCopyDesc& dst,
            size_t                        size = 0);

        /// <summary>
        /// Queue upload texture to texture
        /// </summary>
        void QueueTextureCopy(
            const DeferredTextureCopyDesc& src,
            const DeferredTextureCopyDesc& dst);

        /// <summary>
        /// Queue upload buffer to texture
        /// </summary>
        void QueueUpload(
            const DeferredTransferCopyDesc& copyDesc);

        /// <summary>
        /// Queue readback texture to buffer
        /// </summary>
        void QueueReadback(
            const DeferredTransferCopyDesc& copyDesc);

    public:
        /// <summary>
        /// Flush any pending copy operations
        /// </summary>
        void Flush();

    private:
        /// <summary>
        /// Prepare for copy operations
        /// </summary>
        void PrepareForCopy(
            CommandList& commandList);

        /// <summary>
        /// Perform all copy operations
        /// </summary>
        void DoCopy(
            CommandList& commandList);

        /// <summary>
        /// Restore from copy operations
        /// </summary>
        void RestoreFromCopy(
            CommandList& commandList);

    private:
        class StateStorage;

        struct SavedBufferCopyDesc
        {
            DeferredBufferCopyDesc Src;
            DeferredBufferCopyDesc Dst;
            size_t                 Size;
        };

        struct SavedTextureCopyDesc
        {
            DeferredTextureCopyDesc Src;
            DeferredTextureCopyDesc Dst;
        };

        struct SavedTransferCopyDesc : DeferredTransferCopyDesc
        {
        };

    private:
        Ref<Device>        m_Device;
        UPtr<StateStorage> m_StateStorage;

        std::vector<SavedBufferCopyDesc>   m_BufferCopies;
        std::vector<SavedTextureCopyDesc>  m_TextureCopies;
        std::vector<SavedTransferCopyDesc> m_UploadCopies;
        std::vector<SavedTransferCopyDesc> m_ReadbackCopies;
    };
} // namespace Ame::Rhi::Staging