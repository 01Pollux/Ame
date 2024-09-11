#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    class AME_ENGINE_API MsvcDebugStream final : public ILoggerStream
    {
    public:
        MsvcDebugStream();
    };
} // namespace Ame::Log
