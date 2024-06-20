#include <filesystem>

#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/ShaderCache.hpp>

#include <cryptopp/sha.h>
#include <Rhi/Hash/Shader.Crypto.hpp>

#include <Log/Wrapper.hpp>

namespace bip = boost::interprocess;

namespace Ame::Gfx::Cache
{
    struct ShaderHeader
    {
        static constexpr uint32_t DEFAULT_MAGIC    = 0x414D4553; // "AMES"
        static constexpr uint32_t ENTRY_POINT_SIZE = 24;

        uint32_t Magic = DEFAULT_MAGIC;
        uint32_t DataSize;
    };

    struct ShaderData
    {
        ShaderHeader                       Header;
        bip::managed_mapped_file::handle_t DataHandle;
        char                               EntryPoint[ShaderHeader::ENTRY_POINT_SIZE];
        std::byte                          Data[1];
    };

    struct ShaderDataHelper
    {
        [[nodiscard]] static size_t ComputeBlobSize(
            const Rhi::ShaderBytecode& byteCode)
        {
            return byteCode ? sizeof(ShaderData) + byteCode.GetSize() - 1 : 0;
        }

        [[nodiscard]] static Rhi::ShaderBytecode ToByteCode(
            const bip::managed_mapped_file& mappedFile,
            const ShaderData&               shaderData,
            Rhi::ShaderCompileStage         stage)
        {
            if (shaderData.Header.Magic != ShaderHeader::DEFAULT_MAGIC)
            {
                return {};
            }

            auto byteCode(std::make_unique<std::byte[]>(shaderData.Header.DataSize));
            std::copy(shaderData.Data, shaderData.Data + shaderData.Header.DataSize, byteCode.get());

            return Rhi::ShaderBytecode(shaderData.EntryPoint, std::move(byteCode), shaderData.Header.DataSize, Rhi::CompileStageToShaderType(stage));
        }

        static void FromBytecode(
            const bip::managed_mapped_file& mappedFile,
            ShaderData&                     shaderData,
            const Rhi::ShaderBytecode&      byteCode)
        {
            shaderData.Header.Magic    = ShaderHeader::DEFAULT_MAGIC;
            shaderData.Header.DataSize = static_cast<uint32_t>(byteCode.GetSize());
            std::copy(byteCode.GetBytecode(), byteCode.GetBytecode() + byteCode.GetSize(), shaderData.Data);
        }

        static ShaderData* ToBlob(
            const bip::managed_mapped_file&    mappedFile,
            bip::managed_mapped_file::handle_t handle)
        {
            return std::bit_cast<ShaderData*>(mappedFile.get_address_from_handle(handle));
        }

        static ShaderData* AllocateBlob(
            bip::managed_mapped_file& mappedFile,
            size_t                    size)
        {
            auto shaderData        = std::bit_cast<ShaderData*>(mappedFile.allocate(size));
            shaderData->DataHandle = mappedFile.get_handle_from_address(shaderData);
            return shaderData;
        }

        static bip::managed_mapped_file::handle_t FromBlob(
            const bip::managed_mapped_file& mappedFile,
            ShaderData*                     shaderData)
        {
            return mappedFile.get_handle_from_address(shaderData);
        }
    };

    using CacheMapAllocator = bip::allocator<std::pair<const ShaderCache::PermutationKey, bip::managed_mapped_file::handle_t>, bip::managed_mapped_file::segment_manager>;
    using CacheMap          = bip::map<ShaderCache::PermutationKey, bip::managed_mapped_file::handle_t, std::less<>, CacheMapAllocator>;

    //

    ShaderCache::MappedFileInfo::MappedFileInfo(
        String fileName) :
        File(bip::open_or_create, fileName.c_str(), c_InitialCacheSize),
        FileName(std::move(fileName))
    {
    }

    void ShaderCache::MappedFileInfo::Grow(
        size_t size)
    {
        File.flush();
        File = {};
        MappedFile::grow(this->FileName.c_str(), size);
        File = bip::managed_mapped_file(bip::open_only, this->FileName.c_str());
    }

    //

    Co::result<Rhi::ShaderBytecode> ShaderCache::Load(
        const Rhi::ShaderBuildDesc& desc)
    {
        auto executor = m_Runtime.get().background_executor();

        auto fileTask       = CreateOrOpenFile({}, executor, desc.SourceCode);
        auto permutationKey = GeneratePermutationKey(desc.CompileDesc);

        Rhi::ShaderBytecode result;
        try
        {
            auto mappedFile = co_await fileTask;
            result          = co_await LoadFromCache({}, executor, *mappedFile, permutationKey, desc.CompileDesc);

            if (!result)
            {
                result = co_await CompileAndInsertToCache({}, executor, *mappedFile, desc.SourceCode, permutationKey, desc);
            }
        }
        catch (const std::exception& ex)
        {
            Log::Gfx().Error("Failed to load file from cache: {}", ex.what());
            result = Rhi::ShaderCompiler::Compile(GetShaderResolveDesc(), desc);
        }

        co_return result;
    }

    Co::result<Rhi::ShaderBytecode> ShaderCache::Link(
        const Rhi::ShaderLinkDesc& desc)
    {
        auto executor = m_Runtime.get().background_executor();

        auto fileTask       = CreateOrOpenFile({}, executor, desc.Shaders);
        auto permutationKey = GeneratePermutationKey(desc.CompileDesc);

        Rhi::ShaderBytecode result;
        try
        {
            auto mappedFile = co_await fileTask;
            result          = co_await LoadFromCache({}, executor, *mappedFile, permutationKey, desc.CompileDesc);

            if (!result)
            {
                result = co_await LinkAndInsertToCache({}, executor, *mappedFile, desc.Shaders, permutationKey, desc);
            }
        }
        catch (const std::exception& ex)
        {
            Log::Gfx().Error("Failed to link shaders: {}", ex.what());
            result = Rhi::ShaderCompiler::Link(GetShaderResolveDesc(), desc);
        }

        co_return result;
    }

    //

    auto ShaderCache::CreateOrOpenFile(
        Co::executor_tag,
        const Ptr<Co::executor>& executor,
        StringView               sourceCode) -> Co::result<MappedFileInfo*>
    {
        CryptoPP::SHA256 hasher;
        hasher.Update(c_ShaderSourceHash, std::size(c_ShaderSourceHash));
        hasher.Update(std::bit_cast<const CryptoPP::byte*>(sourceCode.data()), sourceCode.size());
        auto Hash = Util::FinalizeDigestToString(hasher);
        co_return co_await CreateOrOpenFile({}, executor, Hash);
    }

    auto ShaderCache::CreateOrOpenFile(
        Co::executor_tag,
        const Ptr<Co::executor>&             executor,
        std::span<const Rhi::ShaderBytecode> shaders) -> Co::result<MappedFileInfo*>
    {
        CryptoPP::SHA256 hasher;
        hasher.Update(c_LibraryShaderHash, std::size(c_LibraryShaderHash));
        for (auto& Shader : shaders)
        {
            hasher.Update(std::bit_cast<const CryptoPP::byte*>(Shader.GetBytecode()), Shader.GetSize());
        }
        auto hash = Util::FinalizeDigestToString(hasher);
        co_return co_await CreateOrOpenFile({}, executor, hash);
    }

    auto ShaderCache::CreateOrOpenFile(
        Co::executor_tag,
        const Ptr<Co::executor>& executor,
        const String&            fileKey) -> Co::result<MappedFileInfo*>
    {
        auto iter = m_Cache.find(fileKey);
        if (iter == m_Cache.end())
        {
            Co::scoped_async_lock cacheLock = co_await m_Mutex.lock(executor);

            iter = m_Cache.find(fileKey);
            if (iter == m_Cache.end())
            {
                auto fileName = GenerateCacheFileName(m_Device, fileKey);
                iter          = m_Cache.emplace(fileName, std::move(fileName)).first;
            }
        }
        co_return &iter->second;
    }

    //

    Co::result<Rhi::ShaderBytecode> ShaderCache::LoadFromCache(
        Co::executor_tag,
        const Ptr<Co::executor>&      executor,
        MappedFileInfo&               fileInfo,
        const PermutationKey&         permutationKey,
        const Rhi::ShaderCompileDesc& desc)
    {
        Co::scoped_async_lock cacheLock = co_await fileInfo.Mutex.lock(executor);
        CacheMap*             cache     = fileInfo.GetCache<CacheMap>();

        auto iter = cache->find(permutationKey);

        Rhi::ShaderBytecode result;
        if (iter != cache->end())
        {
            auto shaderData = ShaderDataHelper::ToBlob(fileInfo.File, iter->second);
            auto byteCode   = ShaderDataHelper::ToByteCode(fileInfo.File, *shaderData, desc.Stage);
            if (shaderData)
            {
                result = std::move(byteCode);
            }
        }
        co_return result;
    }

    //

    Co::result<Rhi::ShaderBytecode> ShaderCache::CompileAndInsertToCache(
        Co::executor_tag,
        const Ptr<Co::executor>&    executor,
        MappedFileInfo&             fileInfo,
        StringView                  sourceCode,
        const PermutationKey&       permutationKey,
        const Rhi::ShaderBuildDesc& desc)
    {
        auto compileTask = Rhi::ShaderCompiler::Compile(GetShaderResolveDesc(), desc);
        co_return co_await InsertToCache({}, executor, fileInfo, std::move(compileTask), permutationKey, desc.CompileDesc);
    }

    Co::result<Rhi::ShaderBytecode> ShaderCache::LinkAndInsertToCache(
        Co::executor_tag,
        const Ptr<Co::executor>&             executor,
        MappedFileInfo&                      fileInfo,
        std::span<const Rhi::ShaderBytecode> shaders,
        const PermutationKey&                permutationKey,
        const Rhi::ShaderLinkDesc&           desc)
    {
        auto compileTask = Rhi::ShaderCompiler::Link(GetShaderResolveDesc(), desc);
        co_return co_await InsertToCache({}, executor, fileInfo, std::move(compileTask), permutationKey, desc.CompileDesc);
    }

    //

    Co::result<Rhi::ShaderBytecode> ShaderCache::InsertToCache(
        Co::executor_tag,
        const Ptr<Co::executor>&      executor,
        MappedFileInfo&               fileInfo,
        Rhi::ShaderBytecode           compiledShader,
        const PermutationKey&         permutationKey,
        const Rhi::ShaderCompileDesc& desc)
    {
        Co::scoped_async_lock cacheLock = co_await fileInfo.Mutex.lock(executor);
        CacheMap*             cache     = fileInfo.GetCache<CacheMap>();

        ShaderData* shaderData = nullptr;

        auto iter = cache->find(permutationKey);
        if (iter != cache->end())
        {
            shaderData = ShaderDataHelper::ToBlob(fileInfo.File, iter->second);
            fileInfo.File.deallocate(shaderData);
            shaderData = nullptr;
            cache->erase(iter);
        }

        size_t blobSize = ShaderDataHelper::ComputeBlobSize(compiledShader);
        if (blobSize)
        {
            for (uint32_t i = 1; i <= c_GrowAttempts; i++)
            {
                try
                {
                    if (!shaderData)
                    {
                        shaderData = ShaderDataHelper::AllocateBlob(fileInfo.File, blobSize);
                        ShaderDataHelper::FromBytecode(fileInfo.File, *shaderData, compiledShader);
                    }
                    cache->emplace(permutationKey, shaderData->DataHandle);
                    break;
                }
                catch (const bip::bad_alloc&)
                {
                    auto GrowSize = cache->size() * c_PermutationCacheSize + i * c_PermutationGrowSize;
                    fileInfo.Grow(GrowSize);
                    cache = fileInfo.GetCache<CacheMap>();
                }
            }
        }
        co_return compiledShader;
    }

    //

    auto ShaderCache::GeneratePermutationKey(
        const Rhi::ShaderCompileDesc& desc) -> PermutationKey
    {
        CryptoPP::SHA256 hasher;
        Util::UpdateCrypto(hasher, desc);
        return Util::FinalizeDigest(hasher);
    }

    String ShaderCache::GenerateCacheFileName(
        Rhi::Device&  rhiDevice,
        const String& hash)
    {
        return std::format("{}_{}{}.acs", std::filesystem::temp_directory_path().string(), hash, rhiDevice.GetGraphicsAPIName());
    }

    Rhi::ShaderResolveDesc ShaderCache::GetShaderResolveDesc() const
    {
        return {
            .DeviceDesc   = m_Device.get().GetDesc(),
            .AssetStorage = &m_AssetStorage.get()
        };
    }
} // namespace Ame::Gfx::Cache
