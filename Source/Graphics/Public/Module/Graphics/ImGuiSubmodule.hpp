#pragma once

#include <Module/Submodule.hpp>

namespace Ame
{
    // {CF40EE16-0DE3-4095-938A-FB8993CD01FD}
    static const UId IID_ImGuiSubmodule = { 0xcf40ee16, 0xde3, 0x4095, { 0x93, 0x8a, 0xfb, 0x89, 0x93, 0xcd, 0x1, 0xfd } };

    class ImGuiSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_ImGuiSubmodule, IID_BaseSubmodule, Base);

    public:
        explicit ImGuiSubmodule(
            IReferenceCounters* counters) :
            Base(counters, IID_ImGuiSubmodule)
        {
        }
    };
} // namespace Ame