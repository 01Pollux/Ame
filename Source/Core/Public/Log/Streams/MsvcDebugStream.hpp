#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    class MsvcDebugStream final : public ILoggerStream
    {
    public:
        MsvcDebugStream();
    };
} // namespace Ame::Log
