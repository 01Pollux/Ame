#include <Core/Coroutine.hpp>
#include <Log/Logger.hpp>

namespace Ame
{
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