#pragma once

#include <Core/Ame.hpp>
#include <Engine/Logic.hpp>

namespace Ame
{
    class BaseEngine
    {
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
        EngineLogic m_Logic;
    };
} // namespace Ame