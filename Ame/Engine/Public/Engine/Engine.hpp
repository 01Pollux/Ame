#pragma once

namespace Ame
{
    class AmeEngine
    {
    public:
        void Run();

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
        bool m_IsRunning = true;
    };
} // namespace Ame
