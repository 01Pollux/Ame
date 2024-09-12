#pragma once

#include <Scripting/GC.hpp>

namespace Ame::Scripting
{
    // {7E0BBCD0-4F73-4214-9FD1-C498F441F596}
    inline constexpr UId IID_CSGarbageCollector{
        0x7e0bbcd0, 0x4f73, 0x4214, { 0x9f, 0xd1, 0xc4, 0x98, 0xf4, 0x41, 0xf5, 0x96 }
    };

    class CSGarbageCollector : public BaseObject<IGarbageCollector>
    {
    public:
        using Base = BaseObject<IGarbageCollector>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSGarbageCollector, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSGarbageCollector) : Base(counters)
        {
        }

    public:
        void CollectAll() override;
        void Collect(int generation, GCCollectionMode mode = GCCollectionMode::Default, bool blocking = true,
                     bool compacting = false) override;
        void WaitForPendingFinalizers() override;

    private:
    };
} // namespace Ame::Scripting
