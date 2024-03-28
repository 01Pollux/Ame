#pragma once

#include <NRI.h>
#include <Extensions/NRIDeviceCreation.h>

namespace Ame::Rhi
{
    struct NriLogCallbackInterface : public nri::CallbackInterface
    {
        static void MessageCallback(
            nri::Message Type,
            const char*  File,
            uint32_t     Line,
            const char*  Message,
            void*);

        static void AbortExecution(void*);
    };
} // namespace Ame::Rhi