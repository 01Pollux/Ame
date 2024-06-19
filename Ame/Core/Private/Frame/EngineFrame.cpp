#include <Frame/EngineFrame.hpp>

namespace Ame
{
    EngineFrame::EngineFrame(
        FrameTimer& frameTimer) :
        m_Timer(frameTimer)
    {
    }

    void EngineFrame::Run()
    {
        while (m_IsRunning)
        {
            m_Timer.get().Tick();

            m_OnStartFrame();

            if (!IsRunning()) [[unlikely]]
            {
                return;
            }

            m_OnTick();
            m_OnPostTick();

            m_OnEndFrame();
        }
    }

    //

    void EngineFrame::Stop()
    {
        m_IsRunning = false;
    }

    bool EngineFrame::IsRunning() const
    {
        return m_IsRunning;
    }
} // namespace Ame
