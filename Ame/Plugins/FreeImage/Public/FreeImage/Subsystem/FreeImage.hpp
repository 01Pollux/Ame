#pragma once

#include <Core/Subsystem.hpp>

#include <FreeImage/FreeImage.hpp>

namespace Ame::Plugins
{
    struct FreeImageSubsystem : SingleSubsystem<
                                    FreeImageInstance>,
                                kgr::final
    {
    };

    auto service_map(const FreeImageInstance&) -> FreeImageSubsystem;
} // namespace Ame::Plugins
