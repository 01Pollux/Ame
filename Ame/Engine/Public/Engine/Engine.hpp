#pragma once

#include <Core/Container.hpp>
#include <Object/Object.hpp>

#include <Engine/Logic.hpp>
#include <Engine/Timer.hpp>

#include <concurrencpp/concurrencpp.h>

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

    protected:
        EngineLogic m_Logic;
    };
} // namespace Ame