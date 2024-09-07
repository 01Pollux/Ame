#pragma once

#include <Core/Serializable.hpp>

#define AME_ECS_WRAP_COMPONENT_PTR(ComponentType)                                                                      \
    struct ComponentType##Component                                                                                    \
    {                                                                                                                  \
        Ptr<ComponentType> Object;                                                                                     \
    }

#define AME_ECS_WRAP_COMPONENT_REF(ComponentType)                                                                      \
    struct ComponentType##Component                                                                                    \
    {                                                                                                                  \
        ComponentType* Object;                                                                                         \
    }
