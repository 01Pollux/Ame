#pragma once

#include <Core/Ame.hpp>
#include <Engine/Events.hpp>

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
        /// Start the frame
        /// </summary>
        void StartFrame(
            BaseEngine& Engine);

        /// <summary>
        /// Tick all the systems and update the game
        /// </summary>
        void Tick(
            Co::runtime& Runtime,
            BaseEngine&  Engine,
            Rhi::Device* RhiDevice);

        /// <summary>
        /// Tick all the systems and update the game
        /// </summary>
        void Tick(
            BaseEngine& Engine);

        /// <summary>
        /// Render the frame
        /// </summary>
        void EndFrame(
            BaseEngine& Engine);

    public:
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
        AME_SIGNAL_DOUBLE(OnStartFrame);

        AME_SIGNAL_DOUBLE(OnUpdate);
        AME_SIGNAL_DOUBLE(OnPostUpdate);

        AME_SIGNAL_DOUBLE(OnRender);
        AME_SIGNAL_DOUBLE(OnPostRender);

        AME_SIGNAL_DOUBLE(OnEndFrame);

        bool m_IsRunning = true;
    };
} // namespace Ame