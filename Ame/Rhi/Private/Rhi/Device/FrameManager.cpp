#include "FrameManager.hpp"

namespace Ame::Rhi
{
    void FrameManager::NewFrame()
    {
    }

    void FrameManager::EndFrame(
        nri::CommandQueue& GraphicsQueue)
    {
    }

    void FrameManager::FlushIdle()
    {
    }

    //

    uint64_t FrameManager::GetFrameCount() const
    {
        return 0;
    }

    uint8_t FrameManager::GetFrameIndex() const
    {
        return static_cast<uint8_t>(GetFrameCount() % GetFrameCountInFlight());
    }

    uint8_t FrameManager::GetFrameCountInFlight() const
    {
        return 0;
    }
} // namespace Ame::Rhi