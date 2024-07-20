#pragma once

#include <Module/Submodule.hpp>
#include <Core/Coroutine.hpp>
#include <Core/Ame.hpp>

namespace Ame
{
    // {8390DBB1-17D2-49BD-9C90-C70524E3E17A}
    static const UId IID_CoroutineSubmodule = { 0x8390dbb1, 0x17d2, 0x49bd, { 0x9c, 0x90, 0xc7, 0x5, 0x24, 0xe3, 0xe1, 0x7a } };

    class CoroutineSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_CoroutineSubmodule, IID_BaseSubmodule, Base);

    public:
        CoroutineSubmodule(
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