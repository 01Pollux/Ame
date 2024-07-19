#include <Core/FrameTimer.hpp>

namespace Ame
{
    using fmilliseconds = std::chrono::duration<double, std::milli>;

    double FrameTimer::GetGameTime() const noexcept
    {
        return m_GameTime;
    }

    double FrameTimer::GetEngineTime() const noexcept
    {
        return m_EngineTime;
    }

    double FrameTimer::GetDeltaTime() const noexcept
    {
        return m_DeltaTime;
    }

    float FrameTimer::GetTimeScale() const noexcept
    {
        return m_TimeScale;
    }

    bool FrameTimer::IsPaused() const noexcept
    {
        return m_TimeScale <= std::numeric_limits<float>::epsilon();
    }

    void FrameTimer::SetTimeScale(
        float timeScale) noexcept
    {
        // We were paused and now we are unpaused
        if (m_TimeScale <= std::numeric_limits<float>::epsilon() &&
            timeScale > std::numeric_limits<float>::epsilon())
        {
            m_PrevTime = steady_clock::now();
        }
        m_TimeScale = timeScale;
    }

    void FrameTimer::Reset() noexcept
    {
        m_PrevTime = steady_clock::now();
        m_BaseTime = m_PrevTime;

        m_GameTime = 0.0;
    }

    void FrameTimer::Tick() noexcept
    {
        m_CurrTime     = steady_clock::now();
        auto deltaTime = std::max(
            std::chrono::duration_cast<fmilliseconds>(m_CurrTime - m_PrevTime).count() * .001,
            0.0);
        m_DeltaTime = std::min(deltaTime, 0.2) * m_TimeScale;
        m_PrevTime  = m_CurrTime;

        m_GameTime += m_DeltaTime;
        m_EngineTime += m_DeltaTime;
    }
} // namespace Ame