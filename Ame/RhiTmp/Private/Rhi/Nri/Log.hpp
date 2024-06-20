#pragma once

#include "../Nri/Nri.hpp"

namespace Ame::Rhi
{
    struct NriLogCallbackInterface
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