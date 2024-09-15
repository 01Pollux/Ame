#include <CSharpScripting/GC.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.GCBridge, AmeSharp";

    CSGarbageCollector::CSGarbageCollector(IReferenceCounters* counters, const CLRRuntime& runtime) : Base(counters)
    {
        m_CollectAll = runtime.GetFunction<CollectAllFn>(ClassName, "CollectAll");
        m_Collect    = runtime.GetFunction<CollectFn>(ClassName, "Collect");
        m_WaitForPendingFinalizers =
            runtime.GetFunction<WaitForPendingFinalizersFn>(ClassName, "WaitForPendingFinalizers");
    }

    void CSGarbageCollector::CollectAll()
    {
        m_CollectAll();
    }

    void CSGarbageCollector::Collect(int generation, GCCollectionMode mode, bool blocking, bool compacting)
    {
        m_Collect(generation, static_cast<char>(mode), blocking, compacting);
    }

    void CSGarbageCollector::WaitForPendingFinalizers()
    {
        m_WaitForPendingFinalizers();
    }
} // namespace Ame::Scripting