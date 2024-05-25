#include <Rhi/Device/MemoryAllocator.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void MemoryAllocator::Initialize(
        DeviceImpl&                rhiDevice,
        const MemoryAllocatorDesc& memoryAllocatorDesc)
    {
        m_BlockSize       = memoryAllocatorDesc.InitialBlockSize;
        m_GrowSize        = memoryAllocatorDesc.NextBlockSize;
        m_MaxSize         = memoryAllocatorDesc.MaxBlockSize;
        m_GrowthFactor    = memoryAllocatorDesc.GrowthFactor;
        m_MaxGrowAttempts = memoryAllocatorDesc.MaxGrowAttempts;

        m_RhiDevice = &rhiDevice;
    }

    void MemoryAllocator::Shutdown()
    {
        if (!m_BlockHandles.empty() || !m_DedicatedSegments.empty())
        {
            Log::Rhi().Error("MemoryAllocator has leaked resources.");
        }

        m_BlockHandles.clear();
        m_DedicatedSegments.clear();
        m_Node.Release();
    }

    //

    nri::Buffer* MemoryAllocator::CreateBuffer(
        nri::MemoryLocation location,
        const BufferDesc&   desc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nri::Buffer* buffer = nullptr;
        ThrowIfFailed(nriCore.CreateBuffer(m_RhiDevice->GetDevice(), desc, buffer), "failed to create buffer");

        nri::MemoryDesc memoryDesc;
        nriCore.GetBufferMemoryInfo(*buffer, location, memoryDesc);

        if (memoryDesc.mustBeDedicated)
        {
            DedicatedAllocation handle({ AllocateDedicatedMemory(buffer, memoryDesc) }, AllocationType::Buffer);
            BindBufferToMemory(*m_RhiDevice, handle.Memory, *buffer, 0);

            m_DedicatedSegments.emplace(buffer, std::move(handle));
        }
        else
        {
            BlockAllocation handle({ m_Node.Bind(this, *buffer, memoryDesc) }, AllocationType::Buffer);
            if (!handle)
            {
                Log::Rhi().Fatal("failed to create and bind buffer memory");
            }
            m_BlockHandles.emplace(buffer, std::move(handle));
        }

        return buffer;
    }

    nri::Texture* MemoryAllocator::CreateTexture(
        nri::MemoryLocation location,
        const TextureDesc&  desc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nri::Texture* texture = nullptr;
        ThrowIfFailed(nriCore.CreateTexture(m_RhiDevice->GetDevice(), desc, texture), "failed to create texture");

        nri::MemoryDesc memoryDesc;
        nriCore.GetTextureMemoryInfo(*texture, location, memoryDesc);

        if (memoryDesc.mustBeDedicated)
        {
            DedicatedAllocation handle({ AllocateDedicatedMemory(texture, memoryDesc) }, AllocationType::Texture);
            BindTextureToMemory(*m_RhiDevice, handle.Memory, *texture, 0);

            m_DedicatedSegments.emplace(texture, std::move(handle));
        }
        else
        {
            BlockAllocation handle({ m_Node.Bind(this, *texture, memoryDesc) }, AllocationType::Texture);
            if (!handle)
            {
                Log::Rhi().Fatal("failed to create and bind texture memory");
            }
            m_BlockHandles.emplace(texture, std::move(handle));
        }

        return texture;
    }

    //

    void MemoryAllocator::Release(
        void* resource)
    {
        if (auto iter = m_BlockHandles.find(resource); iter != m_BlockHandles.end())
        {
            auto& allocation = iter->second;

            ReleaseOfType(resource, allocation.Type);

            allocation.Allocator->Free(allocation.AllocHandle);
            m_BlockHandles.erase(iter);
        }
        else if (auto iter = m_DedicatedSegments.find(resource); iter != m_DedicatedSegments.end())
        {
            auto& allocation = iter->second;

            ReleaseOfType(resource, allocation.Type);

            auto& nriUtils = m_RhiDevice->GetNRI();
            auto& nriCore  = *nriUtils.GetCoreInterface();

            nriCore.FreeMemory(*allocation.Memory);
            m_DedicatedSegments.erase(iter);
        }
        else
        {
            Log::Rhi().Assert(false, "Tried to release resource which does not exist.");
        }
    }

    void MemoryAllocator::ReleaseOfType(
        void*          resource,
        AllocationType type)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        switch (type)
        {
        case AllocationType::Buffer:
            nriCore.DestroyBuffer(*static_cast<nri::Buffer*>(resource));
            break;
        case AllocationType::Texture:
            nriCore.DestroyTexture(*static_cast<nri::Texture*>(resource));
            break;
        default:
            std::unreachable();
        }
    }

    //

    nri::Memory* MemoryAllocator::AllocateDedicatedMemory(
        void*                  resource,
        const nri::MemoryDesc& desc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nri::Memory* memory = nullptr;
        ThrowIfFailed(nriCore.AllocateMemory(m_RhiDevice->GetDevice(), desc.type, desc.size, memory), "failed to allocate memory");

        return memory;
    }

    void MemoryAllocator::BindBufferToMemory(
        DeviceImpl&  rhiDevice,
        nri::Memory* memory,
        nri::Buffer& buffer,
        size_t       offset)
    {
        auto& nriUtils = rhiDevice.GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nri::BufferMemoryBindingDesc bindingDesc{
            .memory = memory,
            .buffer = &buffer,
            .offset = offset
        };

        ThrowIfFailed(nriCore.BindBufferMemory(rhiDevice.GetDevice(), &bindingDesc, 1), "failed to bind buffer memory");
    }

    void MemoryAllocator::BindTextureToMemory(
        DeviceImpl&   rhiDevice,
        nri::Memory*  memory,
        nri::Texture& texture,
        size_t        offset)
    {
        auto& nriUtils = rhiDevice.GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nri::TextureMemoryBindingDesc bindingDesc{
            .memory  = memory,
            .texture = &texture,
            .offset  = offset
        };

        ThrowIfFailed(nriCore.BindTextureMemory(rhiDevice.GetDevice(), &bindingDesc, 1), "failed to bind texture memory");
    }

    //

    MemoryAllocator::Block::Block(
        DeviceImpl&     rhiDevice,
        size_t          size,
        nri::MemoryType type) :
        RhiDevice(&rhiDevice),
        Allocator(size)
    {
        auto& nriUtils = this->RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.AllocateMemory(this->RhiDevice->GetDevice(), type, size, Memory);
    }

    MemoryAllocator::Block::Block(
        Block&& other) noexcept :
        RhiDevice(std::exchange(other.RhiDevice, nullptr)),
        Memory(std::exchange(other.Memory, nullptr)),
        Allocator(std::move(other.Allocator)),
        SizeLeft(other.SizeLeft)
    {
    }

    MemoryAllocator::Block& MemoryAllocator::Block::operator=(
        Block&& other) noexcept
    {
        if (this != &other)
        {
            Release();
            RhiDevice = std::exchange(other.RhiDevice, nullptr);
            Memory    = std::exchange(other.Memory, nullptr);
            Allocator = std::move(other.Allocator);
            SizeLeft  = other.SizeLeft;
        }
        return *this;
    }

    MemoryAllocator::Block::~Block()
    {
        Release();
    }

    //

    void MemoryAllocator::Block::Release()
    {
        if (Memory)
        {
            auto& nriUtils = RhiDevice->GetNRI();
            auto& nriCore  = *nriUtils.GetCoreInterface();

            nriCore.FreeMemory(*Memory);
            Memory = nullptr;
        }
    }

    //

    auto MemoryAllocator::Block::Bind(
        nri::Buffer&           buffer,
        const nri::MemoryDesc& desc) -> ManagedHandleType
    {
        auto handle = Allocator.Allocate(desc.size, desc.alignment);
        if (handle) [[likely]]
        {
            BindBufferToMemory(*RhiDevice, Memory, buffer, handle.Offset);
        }
        return { &Allocator, handle };
    }

    [[nodiscard]] auto MemoryAllocator::Block::Bind(
        nri::Texture&          texture,
        const nri::MemoryDesc& desc) -> ManagedHandleType
    {
        auto handle = Allocator.Allocate(desc.size, desc.alignment);
        if (handle) [[likely]]
        {
            BindTextureToMemory(*RhiDevice, Memory, texture, handle.Offset);
        }
        return { &Allocator, handle };
    }

    //

    void MemoryAllocator::SegmentRegion::Grow(
        DeviceImpl&      rhiDevice,
        MemoryAllocator* allocator,
        nri::MemoryType  type)
    {
        uint32_t newSize;
        switch (Blocks.size())
        {
        [[unlikely]] case 0:
            newSize = allocator->m_BlockSize;
            break;

        default:
            size_t GrowSize = static_cast<size_t>(allocator->m_GrowSize * allocator->m_GrowthFactor * (Blocks.size() - 1));
            newSize         = std::min(allocator->m_MaxSize, allocator->m_BlockSize + GrowSize);
            break;
        }

        Blocks.emplace_back(rhiDevice, newSize, type);
    }
} // namespace Ame::Rhi