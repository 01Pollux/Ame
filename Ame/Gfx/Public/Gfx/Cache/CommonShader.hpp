#pragma once

#include <map>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <concurrencpp/concurrencpp.h>

#include <Rhi/Resource/Shader.Compiler.hpp>

namespace Ame::Asset
{
    class Storage;
} // namespace Ame::Asset

namespace Ame::Gfx::Cache
{
    class CommonShader
    {
    public:
        enum class Type
        {
            EntityCollectPass_CS,
            GBufferPass_PS,

            Count
        };

        using CacheList     = std::array<Rhi::ShaderBytecode, std::to_underlying(Type::Count)>;
        using CacheLoadList = std::array<Co::result<void>, std::to_underlying(Type::Count)>;

    public:
        CommonShader(
            Co::runtime&    Runtime,
            Asset::Storage& AssetStorage) :
            m_Runtime(Runtime),
            m_AssetStorage(AssetStorage)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Rhi::ShaderBytecode> Load(
            Type ShaderType);

    private:
        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> Create(
            Co::executor&   Executor,
            Asset::Storage& AssetStorage,
            Type            ShaderType);

    private:
        Ref<Co::runtime>    m_Runtime;
        Ref<Asset::Storage> m_AssetStorage;

        CacheList      m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache