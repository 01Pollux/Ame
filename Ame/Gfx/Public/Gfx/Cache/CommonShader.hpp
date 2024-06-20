#pragma once

#include <map>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <concurrencpp/concurrencpp.h>

#include <Rhi/Shader/Shader.hpp>

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
            TiledForward_PS,

            Count
        };

        using CacheList     = std::array<Rhi::ShaderBytecode, std::to_underlying(Type::Count)>;
        using CacheLoadList = std::array<Co::result<void>, std::to_underlying(Type::Count)>;

    public:
        CommonShader(
            Co::runtime&    coroutine,
            Rhi::Device&    rhiDevice,
            Asset::Storage& assetStorage) :
            m_Runtime(coroutine),
            m_Device(rhiDevice),
            m_AssetStorage(assetStorage)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Rhi::ShaderBytecode> Load(
            Type type);

    private:
        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> Create(
            Co::executor&   executor,
            Asset::Storage& assetStorage,
            Type            type);

    private:
        Ref<Co::runtime>    m_Runtime;
        Ref<Rhi::Device>    m_Device;
        Ref<Asset::Storage> m_AssetStorage;

        CacheList      m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache