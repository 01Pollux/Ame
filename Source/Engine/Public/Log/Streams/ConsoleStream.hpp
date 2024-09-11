#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    class AME_ENGINE_API ConsoleStream final : public ILoggerStream
    {
    public:
        ConsoleStream();
    };
} // namespace Ame::Log
