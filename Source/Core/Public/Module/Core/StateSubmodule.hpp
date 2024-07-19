#pragma once

#include <Module/Submodule.hpp>

namespace Ame
{
    // {9BF40EE9-C316-4ABF-95DF-5E82F73676AC}
    static const UId IID_StateSubmodule = { 0x9bf40ee9, 0xc316, 0x4abf, { 0x95, 0xdf, 0x5e, 0x82, 0xf7, 0x36, 0x76, 0xac } };

    class StateSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_StateSubmodule, IID_BaseSubmodule, Base);

    public:
        StateSubmodule(
            IReferenceCounters* counters) :
            Base(counters, IID_StateSubmodule)
        {
        }
    };
} // namespace Ame