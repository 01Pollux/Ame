#pragma once

#include <Ecs/Entity.hpp>

namespace Ame::Ecs::Tag
{
    /// <summary>
    /// Tag pair for entities that are visible to the camera.
    /// first = tag, second = camera bit index.
    /// 0b10010 => { (VisibleToCamera, 1), (VisibleToCamera, 4) }
    /// </summary>
    struct VisibleToCamera
    {
    };
} // namespace Ame::Ecs::Tag
