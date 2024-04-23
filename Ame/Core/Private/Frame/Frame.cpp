#include <Frame/Frame.hpp>

namespace Ame
{
    IFrame::IFrame(
        FrameTimer& Timer) :
        m_Timer(Timer)
    {
    }

    void IFrame::Run()
    {
        while (m_IsRunning)
        {
            m_Timer.get().Tick();
            DoTick();
        }
    }

    //

    void IFrame::Stop()
    {
        m_IsRunning = false;
    }

    bool IFrame::IsRunning() const
    {
        return m_IsRunning;
    }
} // namespace Ame
