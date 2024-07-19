#pragma once

#include <Rhi/Wrapper/DeviceWrapper.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Rhi
{
    struct DeviceCreateDesc;

    enum class ExecutorType : uint8_t
    {
        Primary, // Executor for the main rendering operations

        Graphics,  // Executor for graphics commands
        Compute,   // Executor for compute commands
        Copy,      // Executor for copy commands
        Resources, // Executor for resource creation and destruction

        Count
    };

    class RhiDeviceImpl : public BaseObject<IRhiDevice>
    {
    public:
        using Base = BaseObject<IRhiDevice>;

        void AME_METHOD(QueryInterface)(
            const UId& iid,
            IObject**  outObject) override;

    public:
        RhiDeviceImpl(
            IReferenceCounters*     counters,
            const DeviceCreateDesc& createDesc);

    public:
        void AdvanceFrame(
            uint32_t syncInterval) override;

    private:
        DeviceWrapper m_Wrapper;

        uint32_t m_ConcurrentLoopCount;
        uint32_t m_ConcurrentLoopChunkSize;
    };
} // namespace Ame::Rhi