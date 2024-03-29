#pragma once

#include <Core/Ame.hpp>

namespace Ame
{
    namespace Rhi
    {
        class Device;
    } // namespace Rhi
    class EngineTimer;

    class EngineLogic
    {
    public:
        /// <summary>
        /// Tick all the systems and update the game
        /// </summary>
        void Tick(
            EngineTimer& Timer,
            Rhi::Device* RhiDevice = nullptr);

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