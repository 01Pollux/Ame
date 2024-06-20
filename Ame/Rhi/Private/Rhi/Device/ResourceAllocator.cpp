#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device.Impl.hpp>

namespace Ame::Rhi
{
    DeviceResourceAllocator::DeviceResourceAllocator(
        DeviceImpl& rhiDeviceImpl,
        bool        MultiThreaded) :
        m_RhiDeviceImpl(rhiDeviceImpl),
        m_MultiThreaded(MultiThreaded)
    {
    }

    Co::result<Co::scoped_async_lock> DeviceResourceAllocator::TryLock()
    {
        auto& executor = m_RhiDeviceImpl.get().GetExecutor(ExecutorType::Resources);
        co_return m_MultiThreaded ? Co::scoped_async_lock(co_await m_AsyncLock.lock(executor)) : Co::scoped_async_lock();
    }
} // namespace Ame::Rhi