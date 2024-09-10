#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

#include <EcsComponent/Core.hpp>

namespace Ame::Editor
{
    class IProjectRequest : public IObject
    {
    };

    AME_ECS_WRAP_COMPONENT_REF(IProjectRequest);
} // namespace Ame::Editor