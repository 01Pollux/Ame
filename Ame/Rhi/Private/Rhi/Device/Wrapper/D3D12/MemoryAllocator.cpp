#ifdef AME_PLATFORM_WINDOWS
#include <Rhi/Device/Wrapper/D3D12/MemoryAllocator.hpp>

#include <Core/Enum.hpp>
#include <Rhi/NriError.hpp>

namespace Ame::Rhi::D3D12
{
    static inline const GUID c_D3D12MA_Metadata = { 0x7886d0db, 0x4f86, 0x4caf, { 0xa4, 0x8d, 0xd6, 0x9b, 0x4a, 0x0, 0xa7, 0x4f } };

    //

    D3D12DeviceMemoryAllocator::D3D12DeviceMemoryAllocator(
        const D3D12DeviceMemoryAllocatorDesc& desc) :
        m_NriDevice(desc.NriDevice),
        m_NriCore(desc.NriCore),
        m_NriD3D12(desc.NriD3D12)
    {
        using namespace EnumBitOperators;

        D3D12MA::ALLOCATOR_FLAGS flags = D3D12MA::ALLOCATOR_FLAG_NONE;
        if (!desc.MemoryDesc.get().MultiThreaded)
        {
            flags |= D3D12MA::ALLOCATOR_FLAG_SINGLETHREADED;
        }

        D3D12MA::ALLOCATION_CALLBACKS allocationCallback{
            .pAllocate = [](size_t size, size_t alignment, void*) -> void*
            { return mi_aligned_alloc(alignment, size); },
            .pFree = [](void* ptr, void*) -> void
            { mi_free(ptr); }
        };

        D3D12MA::ALLOCATOR_DESC allocatorDesc{
            .Flags                = flags,
            .pDevice              = desc.Device,
            .PreferredBlockSize   = desc.MemoryDesc.get().BlockSize,
            .pAllocationCallbacks = &allocationCallback,
            .pAdapter             = desc.Adapter
        };

        ThrowIfHrFailed(
            D3D12MA::CreateAllocator(&allocatorDesc, &m_Allocator),
            "Failed to create D3D12 memory allocator.");
    }

    //

    nri::Buffer* D3D12DeviceMemoryAllocator::CreateBuffer(
        MemoryLocation    memoryLocation,
        const BufferDesc& bufferDesc)
    {
        ComPtr<ID3D12Resource>      resource;
        ComPtr<D3D12MA::Allocation> allocation;

        auto                     resourceDesc = GetResourceDesc(bufferDesc);
        D3D12MA::ALLOCATION_DESC allocationDesc{};

        switch (memoryLocation)
        {
        case MemoryLocation::DEVICE:
            allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
            break;
        case MemoryLocation::DEVICE_UPLOAD:
        case MemoryLocation::HOST_UPLOAD:
            allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
            break;
        case MemoryLocation::HOST_READBACK:
            allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
            break;
        }

        m_Allocator->CreateResource(
            &allocationDesc,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            &allocation,
            IID_PPV_ARGS(&resource));

        nri::Buffer*         nriBuffer;
        nri::BufferD3D12Desc d3d12bufferDesc{
            .d3d12Resource   = resource.Get(),
            .structureStride = bufferDesc.structureStride
        };

        ThrowIfFailed(
            m_NriD3D12.get().CreateBufferD3D12(m_NriDevice, d3d12bufferDesc, nriBuffer),
            "Failed to create buffer");

        auto memory = AllocateMemory(resource.Get(), allocation.Get());
        BindMemory(allocation.Get(), memory, nriBuffer);
        SaveAllocation(resource.Get(), { std::move(allocation), memory });

        return nriBuffer;
    }

    void D3D12DeviceMemoryAllocator::ReleaseBuffer(
        nri::Buffer& buffer)
    {
        auto d3d12Buffer = std::bit_cast<ID3D12Resource*>(m_NriCore.get().GetBufferNativeObject(buffer));
        AME_LOG_ASSERT(Log::Rhi(), d3d12Buffer != nullptr, "Buffer resource is null.");

        auto [allocation, memory] = ReleaseAllocation(d3d12Buffer);

        m_NriCore.get().DestroyBuffer(buffer);
        m_NriCore.get().FreeMemory(*memory);
    }

    nri::Texture* D3D12DeviceMemoryAllocator::CreateTexture(
        const TextureDesc& textureDesc)
    {
        ComPtr<ID3D12Resource>      resource;
        ComPtr<D3D12MA::Allocation> allocation;

        auto                     resourceDesc = GetResourceDesc(textureDesc);
        D3D12MA::ALLOCATION_DESC allocationDesc{
            .Flags    = D3D12MA::ALLOCATION_FLAG_NONE,
            .HeapType = D3D12_HEAP_TYPE_DEFAULT
        };

        m_Allocator->CreateResource(
            &allocationDesc,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            &allocation,
            IID_PPV_ARGS(&resource));

        nri::TextureD3D12Desc d3d12TextureDesc{
            .d3d12Resource = resource.Get()
        };

        nri::Texture* nriTexture = nullptr;
        ThrowIfFailed(
            m_NriD3D12.get().CreateTextureD3D12(m_NriDevice, d3d12TextureDesc, nriTexture),
            "Failed to create texture");

        auto memory = AllocateMemory(resource.Get(), allocation.Get());
        BindMemory(allocation.Get(), memory, nriTexture);
        SaveAllocation(resource.Get(), { std::move(allocation), memory });

        return nriTexture;
    }

    void D3D12DeviceMemoryAllocator::ReleaseTexture(
        nri::Texture& texture)
    {
        auto d3d12Texture = std::bit_cast<ID3D12Resource*>(m_NriCore.get().GetTextureNativeObject(texture));
        AME_LOG_ASSERT(Log::Rhi(), d3d12Texture != nullptr, "Texture resource is null.");

        auto [allocation, memory] = ReleaseAllocation(d3d12Texture);

        m_NriCore.get().DestroyTexture(texture);
        m_NriCore.get().FreeMemory(*memory);
    }

    //

    nri::Memory* D3D12DeviceMemoryAllocator::AllocateMemory(
        ID3D12Resource*      resource,
        D3D12MA::Allocation* allocation) const
    {
        D3D12_HEAP_DESC heapDesc{
            .SizeInBytes = allocation->GetSize(),
            .Alignment   = allocation->GetAlignment()
        };
        ThrowIfHrFailed(
            resource->GetHeapProperties(&heapDesc.Properties, &heapDesc.Flags),
            "Failed to get heap properties");

        nri::Memory*         memory;
        nri::MemoryD3D12Desc d3d12MemoryDesc{
            .d3d12Heap     = allocation->GetHeap(),
            .d3d12HeapDesc = &heapDesc
        };

        ThrowIfFailed(
            m_NriD3D12.get().CreateMemoryD3D12(m_NriDevice, d3d12MemoryDesc, memory),
            "Failed to create memory.");
        return memory;
    }

    //

    void D3D12DeviceMemoryAllocator::BindMemory(
        D3D12MA::Allocation* allocation,
        nri::Memory*         memory,
        nri::Buffer*         buffer) const
    {
        nri::BufferMemoryBindingDesc bindMemoryDesc{
            .memory = memory,
            .buffer = buffer,
            .offset = allocation->GetOffset()
        };
        m_NriCore.get().BindBufferMemory(m_NriDevice, &bindMemoryDesc, 1);
    }

    void D3D12DeviceMemoryAllocator::BindMemory(
        D3D12MA::Allocation* allocation,
        nri::Memory*         memory,
        nri::Texture*        texture) const
    {
        nri::TextureMemoryBindingDesc bindMemoryDesc{
            .memory  = memory,
            .texture = texture,
            .offset  = allocation->GetOffset()
        };
        m_NriCore.get().BindTextureMemory(m_NriDevice, &bindMemoryDesc, 1);
    }

    //

    void D3D12DeviceMemoryAllocator::SaveAllocation(
        ID3D12Resource*           resource,
        const AllocationMetadata& metadata)
    {
        // To keep reference up, we need to save reference to allocation in buffer
        ThrowIfHrFailed(
            resource->SetPrivateDataInterface(c_D3D12MA_Metadata, metadata.Allocation.Get()),
            "Failed to set private data.");
        metadata.Allocation->SetPrivateData(metadata.Memory);
    }

    auto D3D12DeviceMemoryAllocator::ReleaseAllocation(
        ID3D12Resource* resource) -> AllocationMetadata
    {
        AllocationMetadata metadata;

        uint32_t size = sizeof(D3D12MA::Allocation*);
        ThrowIfHrFailed(
            resource->GetPrivateData(c_D3D12MA_Metadata, &size, metadata.Allocation.GetAddressOf()),
            "Failed to get private data.");

        ThrowIfHrFailed(
            resource->SetPrivateDataInterface(c_D3D12MA_Metadata, nullptr),
            "Failed to set private data.");

        metadata.Memory = std::bit_cast<nri::Memory*>(metadata.Allocation->GetPrivateData());
        return metadata;
    }
} // namespace Ame::Rhi::D3D12
#endif
