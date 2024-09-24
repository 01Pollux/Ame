#include <Core/Coroutine.hpp>

namespace Ame
{
    struct CoroutineInstance
    {
        Co::runtime* Runtime = new Co::runtime();

        CoroutineInstance();

    private:
        static void HandleOnExit();
    } static s_Instance;

    CoroutineInstance::CoroutineInstance()
    {
        std::atexit(HandleOnExit);
    }

    void CoroutineInstance::HandleOnExit()
    {
        delete s_Instance.Runtime;
    }

    Co::runtime& Coroutine::Get() noexcept
    {
        return *s_Instance.Runtime;
    }
} // namespace Ame