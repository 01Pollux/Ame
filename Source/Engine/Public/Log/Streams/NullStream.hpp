#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    class AME_ENGINE_API NullStream final : public ILoggerStream
    {
    public:
        NullStream();
    };
} // namespace Ame::Log
