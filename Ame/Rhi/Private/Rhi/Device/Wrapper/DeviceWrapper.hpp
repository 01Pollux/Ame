#pragma once

#include <set>

#include <Rhi/Device/CreateDesc.hpp>
#include <Rhi/Device/Wrapper/DeviceMemoryAllocator.hpp>

namespace Ame::Rhi
{
    class DeviceCreateDesc;
    class IDeviceMemoryAllocator;

    class IDeviceWrapper
    {
    public:
        /// <summary>
        /// Create a wrapper for the device.
        /// Uses NRI's nriCreateDeviceFromVkDevice or nriCreateDeviceFromD3D12Device...
        /// </summary>
        [[nodiscard]] static UPtr<IDeviceWrapper> Create(
            const DeviceCreateDesc& createDesc,
            DeviceType              deviceType);

        IDeviceWrapper() = default;

        IDeviceWrapper(const IDeviceWrapper&) = delete;
        IDeviceWrapper(IDeviceWrapper&&)      = delete;

        IDeviceWrapper& operator=(const IDeviceWrapper&) = delete;
        IDeviceWrapper& operator=(IDeviceWrapper&&)      = delete;

        virtual ~IDeviceWrapper();

    public:
        /// <summary>
        /// Get the NRI device.
        /// </summary>
        [[nodiscard]] nri::Device& GetNriDevice() const noexcept;

        /// <summary>
        /// Get the NRI bridge.
        /// </summary>
        [[nodiscard]] const NRIBridge& GetNri() const noexcept;

        /// <summary>
        /// Get the NRI bridge.
        /// </summary>
        [[nodiscard]] NRIBridge& GetNri() noexcept;

    public:
        /// <summary>
        /// Get the NRI queues in a set.
        /// </summary>
        [[nodiscard]] std::set<nri::CommandQueue*> GetQueueSet() noexcept;

        /// <summary>
        /// Get the NRI queue of the specified type.
        /// </summary>
        [[nodiscard]] nri::CommandQueue& GetQueue(
            CommandQueueType queueType) noexcept;

        /// <summary>
        /// Get the NRI graphics queue.
        /// </summary>
        [[nodiscard]] nri::CommandQueue& GetGraphicsQueue() noexcept;

        /// <summary>
        /// Get the NRI compute queue.
        /// </summary>
        [[nodiscard]] nri::CommandQueue& GetComputeQueue() noexcept;

        /// <summary>
        /// Get the NRI copy queue.
        /// </summary>
        [[nodiscard]] nri::CommandQueue& GetCopyQueue() noexcept;

    public:
        /// <summary>
        /// Get the memory allocator.
        /// </summary>
        [[nodiscard]] IDeviceMemoryAllocator& GetMemoryAllocator() noexcept;

    protected:
        /// <summary>
        /// Initialize the device wrapper.
        /// </summary>
        virtual bool Initialize(
            const DeviceCreateDesc& createDesc);

        /// <summary>
        /// Can be called manually or automatically in the destructor only
        /// </summary>
        void Shutdown();

    protected:
        nri::Device* m_Device = nullptr;

        nri::CommandQueue* m_GraphicsQueue = nullptr;
        nri::CommandQueue* m_ComputeQueue  = nullptr;
        nri::CommandQueue* m_CopyQueue     = nullptr;

        NRIBridge m_NRI;

        UPtr<IDeviceMemoryAllocator> m_MemoryAllocator;
    };
} // namespace Ame::Rhi