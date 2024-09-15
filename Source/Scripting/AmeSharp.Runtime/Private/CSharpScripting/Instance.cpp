#include <CSharpScripting/Instance.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.InstanceBridge, AmeSharp";

    CSInstance::CSInstance(IReferenceCounters* counters, const CLRRuntime& runtime, void* instance) :
        Base(counters), m_Runtime(&runtime), m_Instance(instance)
    {
    }

    CSInstance::~CSInstance()
    {
    }
} // namespace Ame::Scripting
