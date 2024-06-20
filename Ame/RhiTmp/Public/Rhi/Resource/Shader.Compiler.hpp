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
            Co::executor&            executor,
            Device&                  rhiDevice,
            StringView               shaderSource,
            const ShaderCompileDesc& desc,
            Asset::Storage*          assetStorage = nullptr);

        [[nodiscard]] static Co::result<ShaderBytecode> LinkAsync(
            Co::executor_tag,
            Co::executor&                   executor,
            Device&                         rhiDevice,
            const ShaderCompileDesc&        linkDesc,
            std::span<const ShaderBytecode> shaders,
            Asset::Storage*                 assetStorage = nullptr);

    public:
        [[nodiscard]] static ShaderBytecode Compile(
            Device&                  rhiDevice,
            StringView               shaderSource,
            const ShaderCompileDesc& desc,
            Asset::Storage*          assetStorage = nullptr);

        [[nodiscard]] static ShaderBytecode Link(
            Device&                         rhiDevice,
            const ShaderCompileDesc&        linkDesc,
            std::span<const ShaderBytecode> shaders,
            Asset::Storage*                 assetStorage = nullptr);
    };
} // namespace Ame::Rhi