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
    struct WorldChange
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

AME_SIGNAL_INSTANCE_DECL(Ame::Ecs::Universe, OnWorldChange, const Data::WorldChange& /*changeData*/);
