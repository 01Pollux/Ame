#pragma once

#include <Core/Ame.hpp>
#include <Object/Signal.hpp>

namespace Ame::Ecs
{
    class Universe;
    class World;
} // namespace Ame::Ecs

namespace Ame::Signals::Data
{
    struct WorldChangeData
    {
        /// <summary>
        /// The old world, can be null.
        /// </summary>
        Ecs::World* OldWorld;

        /// <summary>
        /// The new world, can be null when the world is removed. (shutdown)
        /// </summary>
        Ecs::World* NewWorld;
    };
} // namespace Ame::Signals::Data

AME_SIGNAL_DECL(OnWorldChange, void(const Data::WorldChangeData& changeData));
