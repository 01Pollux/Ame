#include <Frame/AsyncFrame.hpp>

namespace Ame
{
    AsyncFrame::AsyncFrame(
        FrameTimer&  Timer,
        Co::runtime& Runtime) :
        IFrame(Timer),
        m_Runtime(Runtime)
    {
    }

    void AsyncFrame::DoTick()
    {
        auto ThreadExecutor = m_Runtime.get().thread_pool_executor();

        auto DoRender = [this]() -> Co::result<void>
        {
            OnRender().Broadcast();
            OnPostRender().Broadcast();
            co_return;
        };

        OnStartFrame().Broadcast();
        if (!IsRunning())
        {
            return;
        }

        auto RenderTick = ThreadExecutor->submit(DoRender);

        OnUpdate().Broadcast();
        OnPostRender().Broadcast();

        RenderTick.get();

        OnEndFrame().Broadcast();
    }
} // namespace Ame