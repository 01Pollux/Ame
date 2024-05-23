#pragma once

#include <Rhi/Resource/Shader.Desc.hpp>

namespace Ame::Asset
{
    class Storage;
} // namespace Ame::Asset

namespace Ame::Rhi
{
    class ShaderCompiler
    {
    public:
        [[nodiscard]] static Co::result<ShaderBytecode> CompileAsync(
            Co::executor_tag,
            Co::executor&            Executor,
            Device&                  RhiDevice,
            StringView               ShaderSource,
            const ShaderCompileDesc& Desc,
            Asset::Storage*          AssetStorage = nullptr);

        [[nodiscard]] static Co::result<ShaderBytecode> LinkAsync(
            Co::executor_tag,
            Co::executor&                   Executor,
            Device&                         RhiDevice,
            const ShaderCompileDesc&        LinkDesc,
            std::span<const ShaderBytecode> Shaders,
            Asset::Storage*                 AssetStorage = nullptr);

    public:
        [[nodiscard]] static ShaderBytecode Compile(
            Device&                  RhiDevice,
            StringView               ShaderSource,
            const ShaderCompileDesc& Desc,
            Asset::Storage*          AssetStorage = nullptr);

        [[nodiscard]] static ShaderBytecode Link(
            Device&                         RhiDevice,
            const ShaderCompileDesc&        LinkDesc,
            std::span<const ShaderBytecode> Shaders,
            Asset::Storage*                 AssetStorage = nullptr);
    };
} // namespace Ame::Rhi