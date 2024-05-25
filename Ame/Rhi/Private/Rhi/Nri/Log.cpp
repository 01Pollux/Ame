#include "Log.hpp"
#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    void NriLogCallbackInterface::MessageCallback(
        nri::Message type,
        const char*  file,
        uint32_t     line,
        const char*  message,
        void*)
    {
        switch (type)
        {
        case nri::Message::TYPE_INFO:
#ifndef AME_DIST
            Log::Rhi().Info("{}", message);
#endif
            break;
        case nri::Message::TYPE_WARNING:
            Log::Rhi().Warning("{}", message);
            break;
        case nri::Message::TYPE_ERROR:
            Log::Rhi().Error(message);
            break;
        }
    }

    void NriLogCallbackInterface::AbortExecution(void*)
    {
        Log::Rhi().Fatal("GPU Fatal Error");
        AME_DEBUG_BREAK;
        std::abort();
    }
} // namespace Ame::Rhi