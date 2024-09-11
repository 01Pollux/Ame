#pragma once

#include <Core/Interface.hpp>

namespace Ame::Scripting
{
    enum class GCCollectionMode : uint8_t
    {
        // Default is the same as using Forced directly
        Default,

        // Forces the garbage collection to occur immediately
        Forced,

        // Allows the garbage collector to determine whether it should reclaim objects right now
        Optimized,

        // Requests that the garbage collector decommit as much memory as possible
        Aggressive
    };

    class IGarbageCollector
    {
    public:
        virtual void CollectAll()                     = 0;
        virtual void Collect(int generation, GCCollectionMode mode = GCCollectionMode::Default, bool blocking = true,
                             bool compacting = false) = 0;
        virtual void WaitForPendingFinalizers()       = 0;
    };
} // namespace Ame::Scripting
