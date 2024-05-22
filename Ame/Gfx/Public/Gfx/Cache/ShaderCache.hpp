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
    class ShaderCache
    {
        static constexpr const uint8_t ShaderSourceHash[]{ 0x41, 0x53, 0x53 };  // AME SHADER SOURCE
        static constexpr const uint8_t LibraryShaderHash[]{ 0x41, 0x4C, 0x53 }; // AME LIBRARY SHADER

        static constexpr uint32_t PermutationCacheSize  = 10 * 1024; // 10KB
        static constexpr uint32_t PermutationGrowFactor = 10;
        static constexpr uint32_t PermutationGrowSize   = PermutationCacheSize * PermutationGrowFactor;
        static constexpr size_t   InitialCacheSize      = PermutationCacheSize * 50; // 500kb
        static constexpr uint32_t GrowAttempts          = 25;

        using MappedFile = boost::interprocess::managed_mapped_file;
        struct MappedFileInfo
        {
            MappedFile     File;
            Co::async_lock Mutex;
            String         FileName;

            MappedFileInfo(
                String FileName);

            void Grow(
                size_t Size);

            template<typename Ty>
            [[nodiscard]] Ty* GetCache()
            {
                return File.find_or_construct<Ty>("Shaders")(File.get_segment_manager());
            }
        };

    public:
        using PermutationKey = std::array<uint8_t, 32>;
        using FileCacheMap   = std::unordered_map<String, MappedFileInfo>;

    public:
        ShaderCache(
            Rhi::Device&    Device,
            Co::runtime&    Runtime,
            Asset::Storage& Storage) :
            m_Device(Device),
            m_Runtime(Runtime),
            m_AssetStorage(Storage)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Rhi::ShaderBytecode> Load(
            StringView             SourceCode,
            Rhi::ShaderCompileDesc Desc);

        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Rhi::ShaderBytecode> Link(
            Rhi::ShaderCompileDesc           Desc,
            std::vector<Rhi::ShaderBytecode> Shaders);

    private:
        /// <summary>
        /// Create or open a cache file.
        /// </summary>
        [[nodiscard]] Co::result<MappedFileInfo*> CreateOrOpenFile(
            Co::executor_tag,
            const Ptr<Co::executor>& Executor,
            StringView               SourceCode);

        /// <summary>
        /// Create or open a cache file.
        /// </summary>
        [[nodiscard]] Co::result<MappedFileInfo*> CreateOrOpenFile(
            Co::executor_tag,
            const Ptr<Co::executor>&             Executor,
            std::span<const Rhi::ShaderBytecode> Shaders);

        /// <summary>
        /// Create or open a cache file.
        /// </summary>
        [[nodiscard]] Co::result<MappedFileInfo*> CreateOrOpenFile(
            Co::executor_tag,
            const Ptr<Co::executor>& Executor,
            const String&            FileKey);

    private:
        /// <summary>
        /// Load a shader from cache by key.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> LoadFromCache(
            Co::executor_tag,
            const Ptr<Co::executor>&      Executor,
            MappedFileInfo&               FileInfo,
            const PermutationKey&         Key,
            const Rhi::ShaderCompileDesc& Desc);

    private:
        /// <summary>
        /// Load a shader from cache by key.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> CompileAndInsertToCache(
            Co::executor_tag,
            const Ptr<Co::executor>&      Executor,
            MappedFileInfo&               FileInfo,
            StringView                    SourceCode,
            const PermutationKey&         Key,
            const Rhi::ShaderCompileDesc& Desc);

        /// <summary>
        /// Load a shader from cache by key.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> LinkAndInsertToCache(
            Co::executor_tag,
            const Ptr<Co::executor>&             Executor,
            MappedFileInfo&                      FileInfo,
            std::span<const Rhi::ShaderBytecode> Shaders,
            const PermutationKey&                Key,
            const Rhi::ShaderCompileDesc&        Desc);

        /// <summary>
        /// Load a shader from cache by key.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> InsertToCache(
            Co::executor_tag,
            const Ptr<Co::executor>&        Executor,
            MappedFileInfo&                 FileInfo,
            Co::result<Rhi::ShaderBytecode> FinalShader,
            const PermutationKey&           Key,
            const Rhi::ShaderCompileDesc&   Desc);

    private:
        /// <summary>
        /// Generate permutation key from shader compile desc.
        /// </summary>
        [[nodiscard]] static PermutationKey GeneratePermutationKey(
            const Rhi::ShaderCompileDesc& Desc);

        /// <summary>
        /// Generate file cache name
        /// </summary>
        [[nodiscard]] static String GenerateCacheFileName(
            Rhi::Device&  Device,
            const String& Hash);

    private:
        Ref<Rhi::Device>    m_Device;
        Ref<Co::runtime>    m_Runtime;
        Ref<Asset::Storage> m_AssetStorage;

        FileCacheMap   m_Cache;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache