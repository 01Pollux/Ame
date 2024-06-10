#pragma once

#include <Core/Subsystem.hpp>

#include <FreeImage/FreeImage.hpp>

namespace Ame::Extensions
{
    struct FreeImageSubsystem : SingleSubsystem<
                                    FreeImageInstance>,
                                kgr::final
    {
    };

    auto service_map(const FreeImageInstance&) -> FreeImageSubsystem;
} // namespace Ame::Extensions
