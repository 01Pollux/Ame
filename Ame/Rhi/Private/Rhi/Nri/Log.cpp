#include "Log.hpp"
#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    void NriLogCallbackInterface::MessageCallback(
        nri::Message Type,
        const char*  File,
        uint32_t     Line,
        const char*  Message,
        void*)
    {
        switch (Type)
        {
        case nri::Message::TYPE_INFO:
#ifndef AME_DIST
            Log::Rhi().Info("{}", Message);
#endif
            break;
        case nri::Message::TYPE_WARNING:
            Log::Rhi().Warning("{}", Message);
            break;
        case nri::Message::TYPE_ERROR:
            Log::Rhi().Error(Message);
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