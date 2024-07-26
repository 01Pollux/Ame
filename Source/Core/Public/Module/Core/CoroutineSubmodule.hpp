#pragma once

#include <Module/Submodule.hpp>
#include <Core/Coroutine.hpp>
#include <Core/Ame.hpp>

namespace Ame
{
    class CoroutineSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_CoroutineSubmodule, IID_BaseSubmodule, Base);

    public:
        explicit CoroutineSubmodule(
            IReferenceCounters* counters) :
            Base(counters, IID_CoroutineSubmodule)
        {
        }

        [[nodiscard]] Co::runtime& GetRuntime() noexcept
        {
            return m_Runtime;
        }

        [[nodiscard]] const Co::runtime& GetRuntime() const noexcept
        {
            return m_Runtime;
        }

    private:
        Co::runtime m_Runtime;
    };
} // namespace Ame