#pragma once

#include <Rhi/Util/BlockBasedBuffer.hpp>

#include <Rhi/Staging/StagedTexture.hpp>
#include <Rhi/Staging/StagedBuffer.hpp>

namespace Ame::Rhi::Staging
{
    struct DeferredStagingAllocatorDesc
    {
        Util::BlockBasedBufferDesc UploadBufferDesc{
            .Location = MemoryLocation::HOST_UPLOAD
        };
        Util::BlockBasedBufferDesc UploadTextureDesc{
            .Location = MemoryLocation::HOST_UPLOAD
        };
        Util::BlockBasedBufferDesc ReadbackBufferDesc{
            .Location = MemoryLocation::HOST_READBACK
        };
        Util::BlockBasedBufferDesc ReadbackTextureDesc{
            .Location = MemoryLocation::HOST_READBACK
        };
    };

    class DeferredStagingAllocator
    {
    private:
        struct TempAllocation
        {
            Buffer RhiBuffer;
            size_t Offset;
            size_t Size;

            [[nodiscard]] std::byte* GetPtr();
        };

        enum class AllocationType : uint8_t
        {
            Buffer,
            Texture,
            Count
        };

        using BlockBasedTempBuffer = std::array<Util::BlockBasedBuffer, static_cast<int>(AllocationType::Count)>;
        using TempBufferMutexes    = std::array<std::mutex, static_cast<int>(StagedAccessType::Count)>;

    public:
        DeferredStagingAllocator(
            Device&                             rhiDevice,
            const DeferredStagingAllocatorDesc& desc);

    public:
        /// <summary>
        /// Allocate temporary buffer for upload this frame
        /// </summary>
        [[nodiscard]] StagedBuffer AllocateTempBuffer(
            size_t           bufferSize,
            StagedAccessType accessType);

        /// <summary>
        /// Allocate temporary texture for upload this frame
        /// </summary>
        [[nodiscard]] StagedTexture AllocateTempTexture(
            const TextureDesc& desc,
            StagedAccessType   accessType);

    private:
        /// <summary>
        /// Allocate temporary buffer for upload this frame
        /// </summary>
        [[nodiscard]] TempAllocation AllocateTemp(
            size_t           bufferSize,
            StagedAccessType accessType);

        /// <summary>
        /// Allocate temporary texture for upload this frame
        /// </summary>
        [[nodiscard]] TempAllocation AllocateTemp(
            const TextureDesc& desc,
            StagedAccessType   location);

    private:
        /// <summary>
        /// Allocate temporary buffer for upload this frame
        /// </summary>
        [[nodiscard]] TempAllocation AllocateTemp(
            StagedAccessType accessType,
            AllocationType   type,
            size_t           bufferSize);

        /// <summary>
        /// Get the buffer allocator for the specified location and type
        /// </summary>
        [[nodiscard]] std::pair<Util::BlockBasedBuffer*, std::mutex*> GetBufferAllocator(
            StagedAccessType accessType,
            AllocationType   type);

    private:
        Ref<Device> m_Device;

        BlockBasedTempBuffer m_TempUploads;
        BlockBasedTempBuffer m_TempReadbacks;

        TempBufferMutexes m_UploadMutexes;
        TempBufferMutexes m_ReadbackMutexes;
    };
} // namespace Ame::Rhi::Staging