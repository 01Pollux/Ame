#include <Frame/Timer.hpp>

namespace Ame
{
    using fmilliseconds = std::chrono::duration<double, std::milli>;

    double FrameTimer::GetGameTime() const
    {
        return m_GameTime;
    }

    double FrameTimer::GetEngineTime() const
    {
        return m_EngineTime;
    }

    double FrameTimer::GetDeltaTime() const
    {
        return m_DeltaTime;
    }

    float FrameTimer::GetTimeScale() const
    {
        return m_TimeScale;
    }

    void FrameTimer::SetTimeScale(
        float TimeScale)
    {
        // We were paused and now we are unpaused
        if (m_TimeScale <= std::numeric_limits<float>::epsilon() && TimeScale > std::numeric_limits<float>::epsilon())
        {
            m_PrevTime = steady_clock::now();
        }
        m_TimeScale = TimeScale;
    }

    void FrameTimer::Reset()
    {
        m_PrevTime = steady_clock::now();
        m_BaseTime = m_PrevTime;

        m_GameTime = 0.0;
    }

    bool FrameTimer::Tick()
    {
        if (m_TimeScale <= std::numeric_limits<float>::epsilon())
        {
            return false;
        }

        m_CurrTime     = steady_clock::now();
        auto DeltaTime = std::max(
            std::chrono::duration_cast<fmilliseconds>(m_CurrTime - m_PrevTime).count() * .001,
            0.0);
        m_DeltaTime = std::min(DeltaTime, 0.2) * m_TimeScale;
        m_PrevTime  = m_CurrTime;

        m_GameTime += m_DeltaTime;
        m_EngineTime += m_DeltaTime;

        return true;
    }
} // namespace Ame