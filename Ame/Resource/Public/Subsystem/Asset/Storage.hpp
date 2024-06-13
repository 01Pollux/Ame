#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/Coroutine.hpp>

#include <Asset/Storage.hpp>

namespace Ame::Asset
{
    struct StorageSubsystem : SingleSubsystem<
                                  Storage,
                                  Dependency<
                                      CoroutineSubsystem>>,
                              kgr::final
    {
    };

    auto service_map(const Storage&) -> StorageSubsystem;
} // namespace Ame::Asset
