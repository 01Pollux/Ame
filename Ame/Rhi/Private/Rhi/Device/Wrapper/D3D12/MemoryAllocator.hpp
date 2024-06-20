#pragma once

#ifdef AME_PLATFORM_WINDOWS
struct AGSContext;

#include <Rhi/Device/Wrapper/D3D12/Resource.hpp>

namespace Ame::Rhi::D3D12
{
    struct D3D12DeviceMemoryAllocatorDesc
    {
        Ref<nri::Device>                 NriDevice;
        CRef<nri::CoreInterface>         NriCore;
        CRef<nri::WrapperD3D12Interface> NriD3D12;

        CRef<MemoryAllocatorDesc> MemoryDesc;

        ID3D12Device* Device;
        IDXGIAdapter* Adapter;
    };

    class D3D12DeviceMemoryAllocator : public IDeviceMemoryAllocator
    {
        template<typename Ty>
        using ComPtr = Microsoft::WRL::ComPtr<Ty>;

        struct AllocationMetadata
        {
            ComPtr<D3D12MA::Allocation> Allocation;
            nri::Memory*                Memory = nullptr;
        };

    public:
        D3D12DeviceMemoryAllocator(
            const D3D12DeviceMemoryAllocatorDesc& desc);

    public:
        [[nodiscard]] nri::Buffer* CreateBuffer(
            MemoryLocation    memoryLocation,
            const BufferDesc& bufferDesc) override;

        void ReleaseBuffer(
            nri::Buffer& buffer) override;

        [[nodiscard]] nri::Texture* CreateTexture(
            const TextureDesc& textureDesc) override;

        void ReleaseTexture(
            nri::Texture& texture) override;

    private:
        /// <summary>
        /// Allocate memory for the given allocation.
        /// </summary>
        [[nodiscard]] nri::Memory* AllocateMemory(
            ID3D12Resource*      resource,
            D3D12MA::Allocation* allocation) const;

    private:
        /// <summary>
        /// Bind memory to the given buffer.
        /// </summary>
        void BindMemory(
            D3D12MA::Allocation* allocation,
            nri::Memory*         memory,
            nri::Buffer*         buffer) const;

        /// <summary>
        /// Bind memory to the given texture.
        /// </summary>
        void BindMemory(
            D3D12MA::Allocation* allocation,
            nri::Memory*         memory,
            nri::Texture*        texture) const;

    private:
        /// <summary>
        /// Save allocation of the resource to keep it alive
        /// </summary>
        void SaveAllocation(
            ID3D12Resource*           resource,
            const AllocationMetadata& metadata);

        /// <summary>
        /// Prepare resource to be released once the scope end
        /// </summary>
        [[nodiscard]] AllocationMetadata ReleaseAllocation(
            ID3D12Resource* resource);

    private:
        Ref<nri::Device>                 m_NriDevice;
        CRef<nri::CoreInterface>         m_NriCore;
        CRef<nri::WrapperD3D12Interface> m_NriD3D12;

        ComPtr<D3D12MA::Allocator> m_Allocator;
    };
} // namespace Ame::Rhi::D3D12
#endif
