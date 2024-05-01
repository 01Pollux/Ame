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
        Ecs::World& NewWorld;
    };
} // namespace Ame::Signals::Data

AME_SIGNAL_INSTANCE_DECL(Ame::Ecs::Universe, OnWorldChange, const Data::WorldChange& /*ChangeData*/);
