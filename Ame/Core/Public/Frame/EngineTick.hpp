#pragma once

#include <Core/Coroutine.hpp>

namespace Ame
{
    class IEngineTick
    {
    public:
        IEngineTick() = default;

        IEngineTick(const IEngineTick&)            = delete;
        IEngineTick& operator=(const IEngineTick&) = delete;

        IEngineTick(IEngineTick&&)            = delete;
        IEngineTick& operator=(IEngineTick&&) = delete;

        virtual ~IEngineTick() = default;

    public:
        virtual Co::result<void> Tick(
            Co::runtime& runtime) = 0;
    };
} // namespace Ame