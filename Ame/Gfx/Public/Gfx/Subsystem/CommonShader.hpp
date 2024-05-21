#pragma once

#include <Core/Subsystem.hpp>

#include <Core/Subsystem/Coroutine.hpp>
#include <Asset/Subsystem/Storage.hpp>

#include <Gfx/Cache/CommonShader.hpp>

namespace Ame::Gfx::Cache
{
    struct CommonShaderSubsystem : SingleSubsystem<
                                       CommonShader,
                                       Dependency<
                                           CoroutineSubsystem,
                                           Asset::StorageSubsystem>>,
                                   kgr::final
    {
    };

    auto service_map(const CommonShader&) -> CommonShaderSubsystem;
} // namespace Ame::Gfx::Cache
