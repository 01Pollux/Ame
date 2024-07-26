#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

namespace Ame
{
    // {3C5C3A5B-2147-44D9-9DDA-594F891EDA8E}
    static const UId IID_EntityModule = { 0x3c5c3a5b, 0x2147, 0x44d9, { 0x9d, 0xda, 0x59, 0x4f, 0x89, 0x1e, 0xda, 0x8e } };

    struct EcsModuleConfig
    {
        bool Enable3DModule : 1 = true;
    };
} // namespace Ame
