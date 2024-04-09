#pragma once

#include <Core/Container.hpp>
#include <Object/Object.hpp>
#include <Engine/Signals.hpp>

namespace Ame
{
    namespace Rhi
    {
        class Device;
    } // namespace Rhi

    class BaseEngine : public IObject, public Container
    {
        AME_OBJECT(BaseEngine, IObject);

    public:
        virtual ~BaseEngine() = default;

    public:
        /// <summary>
        /// Run the engine
        /// </summary>
        void Run();

        /// <summary>
        /// Mark the engine to close
        /// </summary>
        void Close();

    protected:
        /// <summary>
        /// Initialize the engine
        /// </summary>
        virtual void Initialize();

        /// <summary>
        /// Shutdown the engine
        /// </summary>
        virtual void Shutdown();

    private:
        /// <summary>
        /// Initialize the engine
        /// </summary>
        void DoInitialize();

        /// <summary>
        /// Shutdown the engine
        /// </summary>
        void DoShutdown();

    private:
        /// <summary>
        /// Main engine loop with rendering
        /// </summary>
        void DoRenderLoop(
            Rhi::Device& RhiDevice);

        /// <summary>
        /// Main engine loop without rendering
        /// </summary>
        void DoHeadlessLoop();

    public:
        AME_SIGNAL_DOUBLE(OnStartFrame);

        AME_SIGNAL_DOUBLE(OnUpdate);
        AME_SIGNAL_DOUBLE(OnPostUpdate);

        AME_SIGNAL_DOUBLE(OnRender);
        AME_SIGNAL_DOUBLE(OnPostRender);

        AME_SIGNAL_DOUBLE(OnEndFrame);

    private:
        bool m_IsRunning = true;
    };
} // namespace Ame