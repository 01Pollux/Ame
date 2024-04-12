#include <Rhi/Device/MemoryAllocator.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void MemoryAllocator::Initialize(
        DeviceImpl&                RhiDevice,
        const MemoryAllocatorDesc& Desc)
    {
        m_BlockSize       = Desc.InitialBlockSize;
        m_GrowSize        = Desc.NextBlockSize;
        m_MaxSize         = Desc.MaxBlockSize;
        m_GrowthFactor    = Desc.GrowthFactor;
        m_MaxGrowAttempts = Desc.MaxGrowAttempts;

        m_RhiDevice = &RhiDevice;
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
        nri::MemoryLocation Location,
        const BufferDesc&   Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        nri::Buffer* NriBuffer = nullptr;
        ThrowIfFailed(NriCore.CreateBuffer(m_RhiDevice->GetDevice(), Desc, NriBuffer), "failed to create buffer");

        nri::MemoryDesc MemoryDesc;
        NriCore.GetBufferMemoryInfo(*NriBuffer, Location, MemoryDesc);

        if (MemoryDesc.mustBeDedicated)
        {
            DedicatedAllocation Handle({ AllocateDedicatedMemory(NriBuffer, MemoryDesc) }, AllocationType::Buffer);
            BindBufferToMemory(*m_RhiDevice, Handle.Memory, *NriBuffer, 0);

            m_DedicatedSegments.emplace(NriBuffer, std::move(Handle));
        }
        else
        {
            BlockAllocation Handle({ m_Node.Bind(this, *NriBuffer, MemoryDesc) }, AllocationType::Buffer);
            if (!Handle)
            {
                throw std::runtime_error("failed to create and bind buffer memory");
            }
            m_BlockHandles.emplace(NriBuffer, std::move(Handle));
        }

        return NriBuffer;
    }

    nri::Texture* MemoryAllocator::CreateTexture(
        nri::MemoryLocation Location,
        const TextureDesc&  Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        nri::Texture* NriTexture = nullptr;
        ThrowIfFailed(NriCore.CreateTexture(m_RhiDevice->GetDevice(), Desc, NriTexture), "failed to create texture");

        nri::MemoryDesc MemoryDesc;
        NriCore.GetTextureMemoryInfo(*NriTexture, Location, MemoryDesc);

        if (MemoryDesc.mustBeDedicated)
        {
            DedicatedAllocation Handle({ AllocateDedicatedMemory(NriTexture, MemoryDesc) }, AllocationType::Texture);
            BindTextureToMemory(*m_RhiDevice, Handle.Memory, *NriTexture, 0);

            m_DedicatedSegments.emplace(NriTexture, std::move(Handle));
        }
        else
        {
            BlockAllocation Handle({ m_Node.Bind(this, *NriTexture, MemoryDesc) }, AllocationType::Texture);
            if (!Handle)
            {
                throw std::runtime_error("failed to create and bind texture memory");
            }
            m_BlockHandles.emplace(NriTexture, std::move(Handle));
        }

        return NriTexture;
    }

    //

    void MemoryAllocator::Release(
        void* Resource)
    {
        if (auto Iter = m_BlockHandles.find(Resource); Iter != m_BlockHandles.end())
        {
            auto& Allocation = Iter->second;

            ReleaseOfType(Resource, Allocation.Type);

            Allocation.Allocator->Free(Allocation.AllocHandle);
            m_BlockHandles.erase(Iter);
        }
        else if (auto Iter = m_DedicatedSegments.find(Resource); Iter != m_DedicatedSegments.end())
        {
            auto& Allocation = Iter->second;

            ReleaseOfType(Resource, Allocation.Type);

            auto& Nri     = m_RhiDevice->GetNRI();
            auto& NriCore = *Nri.GetCoreInterface();

            NriCore.FreeMemory(*Allocation.Memory);
            m_DedicatedSegments.erase(Iter);
        }
        else
        {
            Log::Rhi().Assert(false, "Tried to release resource which does not exist.");
        }
    }

    void MemoryAllocator::ReleaseOfType(
        void*          Resource,
        AllocationType Type)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        switch (Type)
        {
        case AllocationType::Buffer:
            NriCore.DestroyBuffer(*static_cast<nri::Buffer*>(Resource));
            break;
        case AllocationType::Texture:
            NriCore.DestroyTexture(*static_cast<nri::Texture*>(Resource));
            break;
        default:
            std::unreachable();
        }
    }

    //

    nri::Memory* MemoryAllocator::AllocateDedicatedMemory(
        void*                  Resource,
        const nri::MemoryDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        nri::Memory* Memory = nullptr;
        ThrowIfFailed(NriCore.AllocateMemory(m_RhiDevice->GetDevice(), Desc.type, Desc.size, Memory), "failed to allocate memory");

        return Memory;
    }

    void MemoryAllocator::BindBufferToMemory(
        DeviceImpl&  RhiDevice,
        nri::Memory* Memory,
        nri::Buffer& Buffer,
        size_t       Offset)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        nri::BufferMemoryBindingDesc BindingDesc{
            .memory = Memory,
            .buffer = &Buffer,
            .offset = Offset
        };

        ThrowIfFailed(NriCore.BindBufferMemory(RhiDevice.GetDevice(), &BindingDesc, 1), "failed to bind buffer memory");
    }

    void MemoryAllocator::BindTextureToMemory(
        DeviceImpl&   RhiDevice,
        nri::Memory*  Memory,
        nri::Texture& Texture,
        size_t        Offset)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        nri::TextureMemoryBindingDesc BindingDesc{
            .memory  = Memory,
            .texture = &Texture,
            .offset  = Offset
        };

        ThrowIfFailed(NriCore.BindTextureMemory(RhiDevice.GetDevice(), &BindingDesc, 1), "failed to bind texture memory");
    }

    //

    MemoryAllocator::Block::Block(
        DeviceImpl&     RhiDevice,
        size_t          Size,
        nri::MemoryType Type) :
        RhiDevice(&RhiDevice),
        Allocator(Size)
    {
        auto& Nri     = this->RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.AllocateMemory(this->RhiDevice->GetDevice(), Type, Size, Memory);
    }

    MemoryAllocator::Block::Block(
        Block&& Other) noexcept :
        RhiDevice(std::exchange(Other.RhiDevice, nullptr)),
        Memory(std::exchange(Other.Memory, nullptr)),
        Allocator(std::move(Other.Allocator)),
        SizeLeft(Other.SizeLeft)
    {
    }

    MemoryAllocator::Block& MemoryAllocator::Block::operator=(Block&& Other) noexcept
    {
        if (this != &Other)
        {
            Release();
            RhiDevice = std::exchange(Other.RhiDevice, nullptr);
            Memory    = std::exchange(Other.Memory, nullptr);
            Allocator = std::move(Other.Allocator);
            SizeLeft  = Other.SizeLeft;
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
            auto& Nri     = RhiDevice->GetNRI();
            auto& NriCore = *Nri.GetCoreInterface();

            NriCore.FreeMemory(*Memory);
            Memory = nullptr;
        }
    }

    //

    auto MemoryAllocator::Block::Bind(
        nri::Buffer&           Buffer,
        const nri::MemoryDesc& Desc) -> ManagedHandleType
    {
        auto Handle = Allocator.Allocate(Desc.size, Desc.alignment);
        if (Handle) [[likely]]
        {
            BindBufferToMemory(*RhiDevice, Memory, Buffer, Handle.Offset);
        }
        return { &Allocator, Handle };
    }

    [[nodiscard]] auto MemoryAllocator::Block::Bind(
        nri::Texture&          Texture,
        const nri::MemoryDesc& Desc) -> ManagedHandleType
    {
        auto Handle = Allocator.Allocate(Desc.size, Desc.alignment);
        if (Handle) [[likely]]
        {
            BindTextureToMemory(*RhiDevice, Memory, Texture, Handle.Offset);
        }
        return { &Allocator, Handle };
    }

    //

    void MemoryAllocator::SegmentRegion::Grow(
        DeviceImpl&      RhiDevice,
        MemoryAllocator* Allocator,
        nri::MemoryType  Type)
    {
        uint32_t NewSize;
        switch (Blocks.size())
        {
        [[unlikely]] case 0:
            NewSize = Allocator->m_BlockSize;
            break;

        default:
            size_t GrowSize = static_cast<size_t>(Allocator->m_GrowSize * Allocator->m_GrowthFactor * (Blocks.size() - 1));
            NewSize         = std::min(Allocator->m_MaxSize, Allocator->m_BlockSize + GrowSize);
            break;
        }

        Blocks.emplace_back(RhiDevice, NewSize, Type);
    }
} // namespace Ame::Rhi