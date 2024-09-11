#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    class ConsoleStream final : public ILoggerStream
    {
    public:
        ConsoleStream();
    };
} // namespace Ame::Log
