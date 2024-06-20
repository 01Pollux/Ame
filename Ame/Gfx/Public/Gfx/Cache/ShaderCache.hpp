#pragma once

#include <map>
#include <boost/interprocess/managed_mapped_file.hpp>

#include <Core/Coroutine.hpp>
#include <Rhi/Shader/Shader.Compiler.hpp>

namespace Ame::Asset
{
    class Storage;
} // namespace Ame::Asset

namespace Ame::Gfx::Cache
{
    class ShaderCache
    {
        static constexpr const uint8_t c_ShaderSourceHash[]{ 0x41, 0x53, 0x53 };  // AME SHADER SOURCE
        static constexpr const uint8_t c_LibraryShaderHash[]{ 0x41, 0x4C, 0x53 }; // AME LIBRARY SHADER

        static constexpr uint32_t c_PermutationCacheSize  = 10 * 1024; // 10KB
        static constexpr uint32_t c_PermutationGrowFactor = 10;
        static constexpr uint32_t c_PermutationGrowSize   = c_PermutationCacheSize * c_PermutationGrowFactor;
        static constexpr size_t   c_InitialCacheSize      = c_PermutationCacheSize * 50; // 500kb
        static constexpr uint32_t c_GrowAttempts          = 25;

        using MappedFile = boost::interprocess::managed_mapped_file;
        struct MappedFileInfo
        {
            MappedFile     File;
            Co::async_lock Mutex;
            String         FileName;

            MappedFileInfo(
                String fileName);

            void Grow(
                size_t size);

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
            const Rhi::ShaderBuildDesc& desc);

        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Rhi::ShaderBytecode> Link(
            const Rhi::ShaderLinkDesc& desc);

    private:
        /// <summary>
        /// Create or open a cache file.
        /// </summary>
        [[nodiscard]] Co::result<MappedFileInfo*> CreateOrOpenFile(
            Co::executor_tag,
            const Ptr<Co::executor>& executor,
            StringView               sourceCode);

        /// <summary>
        /// Create or open a cache file.
        /// </summary>
        [[nodiscard]] Co::result<MappedFileInfo*> CreateOrOpenFile(
            Co::executor_tag,
            const Ptr<Co::executor>&             executor,
            std::span<const Rhi::ShaderBytecode> shaders);

        /// <summary>
        /// Create or open a cache file.
        /// </summary>
        [[nodiscard]] Co::result<MappedFileInfo*> CreateOrOpenFile(
            Co::executor_tag,
            const Ptr<Co::executor>& executor,
            const String&            fileKey);

    private:
        /// <summary>
        /// Load a shader from cache by key.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> LoadFromCache(
            Co::executor_tag,
            const Ptr<Co::executor>&      executor,
            MappedFileInfo&               fileInfo,
            const PermutationKey&         permutationKey,
            const Rhi::ShaderCompileDesc& desc);

    private:
        /// <summary>
        /// Load a shader from cache by key.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> CompileAndInsertToCache(
            Co::executor_tag,
            const Ptr<Co::executor>&    executor,
            MappedFileInfo&             fileInfo,
            StringView                  sourceCode,
            const PermutationKey&       permutationKey,
            const Rhi::ShaderBuildDesc& desc);

        /// <summary>
        /// Load a shader from cache by key.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> LinkAndInsertToCache(
            Co::executor_tag,
            const Ptr<Co::executor>&             executor,
            MappedFileInfo&                      fileInfo,
            std::span<const Rhi::ShaderBytecode> shaders,
            const PermutationKey&                permutationKey,
            const Rhi::ShaderLinkDesc&           desc);

    public:
        /// <summary>
        /// Load a shader from cache by key.
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> InsertToCache(
            Co::executor_tag,
            const Ptr<Co::executor>&      executor,
            MappedFileInfo&               fileInfo,
            Rhi::ShaderBytecode           compiledShader,
            const PermutationKey&         permutationKey,
            const Rhi::ShaderCompileDesc& desc);

    private:
        /// <summary>
        /// Generate permutation key from shader compile desc.
        /// </summary>
        [[nodiscard]] static PermutationKey GeneratePermutationKey(
            const Rhi::ShaderCompileDesc& desc);

        /// <summary>
        /// Generate file cache name
        /// </summary>
        [[nodiscard]] static String GenerateCacheFileName(
            Rhi::Device&  rhiDevice,
            const String& shaderHash);

        /// <summary>
        /// Get shader resolve desc
        /// </summary>
        [[nodiscard]] Rhi::ShaderResolveDesc GetShaderResolveDesc() const;

    private:
        Ref<Co::runtime>    m_Runtime;
        Ref<Rhi::Device>    m_Device;
        Ref<Asset::Storage> m_AssetStorage;

        FileCacheMap   m_Cache;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache