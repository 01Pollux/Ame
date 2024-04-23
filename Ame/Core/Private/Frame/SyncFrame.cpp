#include <Frame/SyncFrame.hpp>

namespace Ame
{
    void SyncFrame::DoTick()
    {
        OnStartFrame().Broadcast();

        if (!IsRunning())
        {
            return;
        }

        OnUpdate().Broadcast();
        OnPostUpdate().Broadcast();

        OnRender().Broadcast();
        OnPostRender().Broadcast();

        OnEndFrame().Broadcast();
    }
} // namespace Ame