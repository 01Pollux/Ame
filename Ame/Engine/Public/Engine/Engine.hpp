#pragma once

#include <Core/Ame.hpp>
#include <Object/Object.hpp>

#include <Engine/Logic.hpp>
#include <Engine/Timer.hpp>

#include <concurrencpp/concurrencpp.h>
#include <Rhi/Device.hpp>

namespace Ame
{
    class BaseEngine : public IObject
    {
        AME_OBJECT(BaseEngine, IObject);

    public:
        BaseEngine(
            Rhi::Device      RhiDevice,
            Ptr<Co::runtime> Runtime);

        virtual ~BaseEngine();

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
        virtual void Initialize()
        {
        }

        /// <summary>
        /// Shutdown the engine
        /// </summary>
        virtual void Shutdown()
        {
        }

    private:
        /// <summary>
        /// Main engine loop with rendering
        /// </summary>
        void RenderLoop();

        /// <summary>
        /// Main engine loop without rendering
        /// </summary>
        void HeadlessLoop();

    protected:
        EngineTimer      m_Timer;
        Ptr<Co::runtime> m_Runtime;
        Rhi::Device      m_RhiDevice;
        EngineLogic      m_Logic;
    };
} // namespace Ame