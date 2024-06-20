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
        class Device;
    } // namespace Rhi

    template<typename Ty>
    concept AmeEngine = requires (Ty engine)
	{
		{ engine.Run() } -> std::same_as<Co::result<void>>;
        std::derived_from<Ty, IoCContainer>;
	};
} // namespace Ame