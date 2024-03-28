#pragma once

#include <Core/Ame.hpp>

namespace Ame
{
    class EngineLogic
    {
    public:
        /// <summary>
        /// Tick all the systems and update the game
        /// </summary>
        void Tick();

        /// <summary>
        /// Stop the engine loop
        /// </summary>
        void Stop();

        /// <summary>
        /// Check if the engine loop is running
        /// </summary>
        [[nodiscard]] bool IsRunning() const
        {
            return m_IsRunning;
        }

    public:
        /// <summary>
        /// Initialize all the systems
        /// </summary>
        void Initialize();

        /// <summary>
        /// Shutdown all the systems
        /// </summary>
        void Shutdown();

    private:
        bool m_IsRunning = true;
    };
} // namespace Ame