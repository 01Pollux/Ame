#pragma once

#include <iosfwd>
#include <Log/Stream.hpp>

namespace Ame::Log
{
    class StdStream final : public ILoggerStream
    {
    public:
        StdStream(std::ostream& os, bool forceFlush = false);
    };
} // namespace Ame::Log
