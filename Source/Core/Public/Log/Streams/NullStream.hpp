#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    class NullStream final : public ILoggerStream
    {
    public:
        NullStream();
    };
} // namespace Ame::Log
