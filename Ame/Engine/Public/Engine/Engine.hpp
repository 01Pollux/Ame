#pragma once

#include <Core/Container.hpp>
#include <Core/String.hpp>

namespace Ame
{
    namespace Gfx
    {
        class Renderer;
    } // namespace Gfx

    namespace Rhi
    {
        class Device;
    } // namespace Rhi

    class BaseEngine : public Container
    {
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
        /// Initialize the engine
        /// </summary>
        void DoInitialize();

        /// <summary>
        /// Shutdown the engine
        /// </summary>
        void DoShutdown();
    };
} // namespace Ame