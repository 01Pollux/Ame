#pragma once

#include <Core/Coroutine.hpp>
#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi
{
    class DeviceResourceAllocator
    {
    public:
        DeviceResourceAllocator(
            class DeviceImpl& rhiDeviceImpl,
            bool              ThreadSafe);

    public:
        /// <summary>
        /// Get the rhi device desc.
        /// </summary>
        [[nodiscard]] const Rhi::DeviceDesc& GetDeviceDesc() const noexcept;

        /// <summary>
        /// Get the nri core interface.
        /// </summary>
        [[nodiscard]] nri::CoreInterface& GetNriCore() const noexcept;

        /// <summary>
        /// Get the frame manager.
        /// </summary>
        [[nodiscard]] FrameManager& GetFrameManager() const noexcept;

        /// <summary>
        /// Get the rhi device wrapper.
        /// </summary>
        [[nodiscard]] IDeviceWrapper& GetWrapper() const noexcept;

    public:
        /// <summary>
        /// Create a pipeline layout.
        /// </summary>
        [[nodiscard]] PipelineLayout CreatePipelineLayout(
            const PipelineLayoutDesc& desc);

    public:
        /// <summary>
        /// Create a pipeline state object.
        /// </summary>
        [[nodiscard]] PipelineState CreatePipelineState(
            const GraphicsPipelineDesc& desc);

        /// <summary>
        /// Create a pipeline state object.
        /// </summary>
        [[nodiscard]] PipelineState CreatePipelineState(
            const ComputePipelineDesc& desc);

    public:
        /// <summary>
        /// Create a buffer.
        /// </summary>
        [[nodiscard]] Buffer CreateBuffer(
            const BufferDesc& desc,
            MemoryLocation    location);

        /// <summary>
        /// Create a texture.
        /// </summary>
        [[nodiscard]] Texture CreateTexture(
            const TextureDesc& desc);

    public:
        /// <summary>
        /// Create a sampler resource view.
        /// </summary>
        [[nodiscard]] ResourceView CreateSampler(
            const SamplerDesc& desc);

    public:
        /// <summary>
        /// Create a descriptor table.
        /// </summary>
        [[nodiscard]] DescriptorTable CreateDescriptorTable(
            const DescriptorPoolDesc& desc);

    private:
        struct LockGuard
        {
            LockGuard(
                std::mutex& mutex,
                bool        shouldLock) :
                m_Mutex(mutex),
                m_Locked(shouldLock)
            {
                if (shouldLock)
                {
                    m_Mutex.lock();
                }
            }

            LockGuard(const LockGuard&) = delete;
            LockGuard(LockGuard&&)      = delete;

            LockGuard& operator=(LockGuard&&)      = delete;
            LockGuard& operator=(const LockGuard&) = delete;

            ~LockGuard()
            {
                if (m_Locked)
                {
                    m_Mutex.unlock();
                }
            }

        private:
            std::mutex& m_Mutex;
            bool        m_Locked = false;
        };

    public:
        /// <summary>
        /// Lock the resource allocator if it is thread-safe.
        /// Used internally for deferring resource releases.
        /// </summary>
        [[nodiscard]] LockGuard TryLock()
        {
            return LockGuard(m_Mutex, m_MultiThreaded);
        }

    private:
        Ref<DeviceImpl> m_RhiDeviceImpl;

        std::mutex m_Mutex;
        bool       m_MultiThreaded = false;
    };
} // namespace Ame::Rhi