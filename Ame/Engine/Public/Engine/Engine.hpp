#pragma once

#include <Core/Ame.hpp>
#include <Engine/Logic.hpp>
#include <Engine/Timer.hpp>
#include <Rhi/Device.hpp>

namespace Ame
{
    class BaseEngine : public NonCopyable,
                       public NonMovable
    {
    public:
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
        EngineTimer m_Timer;
        Rhi::Device m_RhiDevice;
        EngineLogic m_Logic;
    };
} // namespace Ame