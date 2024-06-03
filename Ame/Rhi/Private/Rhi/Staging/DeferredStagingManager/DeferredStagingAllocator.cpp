#include <Rhi/Staging/DeferredStagingManager/DeferredStagingAllocator.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Util/ResourceSize.hpp>

namespace Ame::Rhi::Staging
{
    std::byte* DeferredStagingAllocator::TempAllocation::GetPtr()
    {
        return RhiBuffer.GetPtr(Offset);
    }

    //

    DeferredStagingAllocator::DeferredStagingAllocator(
        Device&                             rhiDevice,
        const DeferredStagingAllocatorDesc& desc) :
        m_Device(rhiDevice),
        m_TempUploads{
            TempBlockBuffer{ rhiDevice, desc.UploadBufferDesc },
            TempBlockBuffer{ rhiDevice, desc.UploadTextureDesc }
        },
        m_TempReadbacks{
            TempBlockBuffer{ rhiDevice, desc.ReadbackBufferDesc },
            TempBlockBuffer{ rhiDevice, desc.ReadbackTextureDesc }
        }
    {
    }

    //

    StagedBuffer DeferredStagingAllocator::AllocateTempBuffer(
        size_t           bufferSize,
        StagedAccessType accessType)
    {
        auto allocation = AllocateTemp(bufferSize, accessType);
        return StagedBuffer(
            std::move(allocation.RhiBuffer),
            allocation.Offset,
            allocation.Size);
    }

    StagedTexture DeferredStagingAllocator::AllocateTempTexture(
        const TextureDesc& desc,
        StagedAccessType   accessType)
    {
        auto allocation = AllocateTemp(desc, accessType);
        return StagedTexture(
            m_Device,
            desc,
            allocation.Size,
            allocation.Offset,
            std::move(allocation.RhiBuffer));
    }

    //

    auto DeferredStagingAllocator::AllocateTemp(
        size_t           bufferSize,
        StagedAccessType accessType)
        -> TempAllocation
    {
        return AllocateTemp(
            accessType,
            AllocationType::Buffer,
            bufferSize);
    }

    auto DeferredStagingAllocator::AllocateTemp(
        const TextureDesc& desc,
        StagedAccessType   accessType)
        -> TempAllocation
    {
        return AllocateTemp(
            accessType,
            AllocationType::Texture,
            Util::GetUploadBufferTextureSize(
                m_Device.get().GetDesc(),
                desc.format,
                desc.width,
                desc.height,
                desc.depth,
                desc.mipNum,
                desc.arraySize));
    }

    //

    auto DeferredStagingAllocator::AllocateTemp(
        StagedAccessType accessType,
        AllocationType   type,
        size_t           bufferSize) -> TempAllocation
    {
        auto [allocator, mutex] = GetBufferAllocator(accessType, type);
        std::scoped_lock lock(*mutex);

        auto  handle = allocator->Rent(bufferSize);
        auto& buffer = allocator->GetBuffer(handle);

        TempAllocation allocation{
            .RhiBuffer = buffer,
            .Offset    = handle.Offset,
            .Size      = bufferSize
        };
        allocator->Return(handle);

        return allocation;
    }

    auto DeferredStagingAllocator::GetBufferAllocator(
        StagedAccessType accessType,
        AllocationType   type) -> std::pair<TempBlockBuffer*, std::mutex*>
    {
        int index = static_cast<int>(type);
        switch (accessType)
        {
        case StagedAccessType::Write:
            return { &m_TempUploads[index], &m_UploadMutexes[index] };
        case StagedAccessType::Read:
            return { &m_TempReadbacks[index], &m_ReadbackMutexes[index] };
        default:
            std::unreachable();
        }
    }
} // namespace Ame::Rhi::Staging