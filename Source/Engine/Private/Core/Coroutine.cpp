#include <Core/Coroutine.hpp>
#include <Log/Logger.hpp>

namespace Ame
{
    static Ptr<Coroutine> s_Coroutine;

    Co::runtime& Coroutine::Get() noexcept
    {
        return s_Coroutine->m_Runtime;
    }

    void Coroutine::Initialize()
    {
        s_Coroutine = AmeCreate(Coroutine);
    }

    void Coroutine::Shutdown()
    {
        AME_LOG_ASSERT(
            s_Coroutine->GetReferenceCounters()->GetNumStrongRefs() == 1, "Coroutine reference count is not 1");
        s_Coroutine = nullptr;
    }
} // namespace Ame