#pragma once

#include <Core/Container.hpp>
#include <Core/Coroutine.hpp>
#include <Core/String.hpp>

namespace Ame
{
    namespace Gfx
    {
        class Renderer;
    } // namespace Gfx

    namespace Rhi
    {
        class RhiDevice;
    } // namespace Rhi

    class BaseAmeEngine
    {
    public:
        BaseAmeEngine(
            kgr::container container) :
            m_Container(std::move(container))
        {
        }

        BaseAmeEngine(const BaseAmeEngine&) = delete;
        BaseAmeEngine(BaseAmeEngine&&)      = default;

        BaseAmeEngine& operator=(const BaseAmeEngine&) = delete;
        BaseAmeEngine& operator=(BaseAmeEngine&&)      = default;

        virtual ~BaseAmeEngine() = default;

    public:
        /// <summary>
        /// Run the engine.
        /// </summary>
        [[nodiscard]] Co::result<void> Run();

    protected:
        /// <summary>
        /// Initialize the engine.
        /// </summary>
        [[nodiscard]] virtual Co::result<void> Initialize();

        /// <summary>
        /// Shutdown the engine.
        /// </summary>
        [[nodiscard]] virtual Co::result<void> Shutdown();

    protected:
        kgr::container m_Container;
    };
} // namespace Ame