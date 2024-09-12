#pragma once

#include <Scripting/GC.hpp>

namespace Ame::Scripting
{
    class CSGarbageCollector : public IGarbageCollector
    {
    public:
        void CollectAll() override;
        void Collect(int generation, GCCollectionMode mode = GCCollectionMode::Default, bool blocking = true,
                     bool compacting = false) override;
        void WaitForPendingFinalizers() override;

    private:
    };
} // namespace Ame::Scripting
