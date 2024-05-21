#include <filesystem>

#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/ShaderCache.hpp>

#include <cryptopp/sha.h>
#include <Util/Crypto.hpp>

namespace bip = boost::interprocess;

namespace Ame::Gfx::Cache
{
    struct ShaderHeader
    {
        static constexpr uint32_t DEFAULT_MAGIC = 0x414D4553; // "AMES"

        uint32_t Magic = DEFAULT_MAGIC;
        uint32_t DataSize;
    };

    struct ShaderData
    {
        ShaderHeader                       Header;
        bip::managed_mapped_file::handle_t DataHandle;
        uint8_t                            Data[1];
    };

    struct ShaderDataHelper
    {
        [[nodiscard]] static size_t CalculateBlobSize(
            const Rhi::ShaderBytecode& ByteCode)
        {
            return ByteCode ? sizeof(ShaderData) + ByteCode.GetSize() - 1 : 0;
        }

        [[nodiscard]] static Rhi::ShaderBytecode ToByteCode(
            const bip::managed_mapped_file& File,
            const ShaderData&               Blob,
            Rhi::ShaderType                 Stage)
        {
            if (Blob.Header.Magic != ShaderHeader::DEFAULT_MAGIC)
            {
                return {};
            }

            auto ByteCode(std::make_unique<uint8_t[]>(Blob.Header.DataSize));
            std::copy(Blob.Data, Blob.Data + Blob.Header.DataSize, ByteCode.get());

            return Rhi::ShaderBytecode(ByteCode.release(), Blob.Header.DataSize, Stage);
        }

        static void FromBytecode(
            const bip::managed_mapped_file& File,
            ShaderData&                     Blob,
            const Rhi::ShaderBytecode&      ByteCode)
        {
            Blob.Header.Magic    = ShaderHeader::DEFAULT_MAGIC;
            Blob.Header.DataSize = static_cast<uint32_t>(ByteCode.GetSize());
            std::copy(ByteCode.GetBytecode(), ByteCode.GetBytecode() + ByteCode.GetSize(), Blob.Data);
        }

        static ShaderData* ToBlob(
            const bip::managed_mapped_file&    File,
            bip::managed_mapped_file::handle_t Handle)
        {
            return std::bit_cast<ShaderData*>(File.get_address_from_handle(Handle));
        }

        static ShaderData* AllocateBlob(
            bip::managed_mapped_file& File,
            size_t                    Size)
        {
            auto Blob        = std::bit_cast<ShaderData*>(File.allocate(Size));
            Blob->DataHandle = File.get_handle_from_address(Blob);
            return Blob;
        }

        static bip::managed_mapped_file::handle_t FromBlob(
            const bip::managed_mapped_file& File,
            ShaderData*                     Blob)
        {
            return File.get_handle_from_address(Blob);
        }
    };

    using CacheMapAllocator = bip::allocator<std::pair<const ShaderCache::PermutationKey, bip::managed_mapped_file::handle_t>, bip::managed_mapped_file::segment_manager>;
    using CacheMap          = bip::map<ShaderCache::PermutationKey, bip::managed_mapped_file::handle_t, std::less<>, CacheMapAllocator>;

    //

    ShaderCache::MappedFileInfo::MappedFileInfo(
        String FileName) :
        File(bip::open_or_create, FileName.c_str(), InitialCacheSize),
        FileName(std::move(FileName))
    {
    }

    void ShaderCache::MappedFileInfo::Grow(
        size_t Size)
    {
        File.flush();
        File = {};
        MappedFile::grow(this->FileName.c_str(), Size);
        File = bip::managed_mapped_file(bip::open_only, this->FileName.c_str());
    }

    //

    Co::result<Rhi::ShaderBytecode> ShaderCache::Load(
        StringView             SourceCode,
        Rhi::ShaderCompileDesc Desc)
    {
        auto Executor = m_Runtime.get().background_executor();

        auto FileTask = CreateOrOpenFile({}, Executor, SourceCode);
        auto Key      = GeneratePermutationKey(Desc);

        auto File   = co_await FileTask;
        auto Result = co_await LoadFromCache({}, Executor, *File, Key, Desc);

        if (!Result)
        {
            Result = co_await CompileAndInsertToCache({}, Executor, *File, SourceCode, Key, Desc);
        }
        co_return Result;
    }

    Co::result<Rhi::ShaderBytecode> ShaderCache::Link(
        Rhi::ShaderCompileDesc           Desc,
        std::vector<Rhi::ShaderBytecode> Shaders)
    {
        auto Executor = m_Runtime.get().background_executor();

        auto FileTask = CreateOrOpenFile({}, Executor, Shaders);
        auto Key      = GeneratePermutationKey(Desc);

        auto File   = co_await FileTask;
        auto Result = co_await LoadFromCache({}, Executor, *File, Key, Desc);

        if (!Result)
        {
            Result = co_await LinkAndInsertToCache({}, Executor, *File, Shaders, Key, Desc);
        }
        co_return Result;
    }

    //

    auto ShaderCache::CreateOrOpenFile(
        Co::executor_tag,
        const Ptr<Co::executor>& Executor,
        StringView               SourceCode) -> Co::result<MappedFileInfo*>
    {
        CryptoPP::SHA256 Hasher;
        Hasher.Update(ShaderSourceHash, std::size(ShaderSourceHash));
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(SourceCode.data()), SourceCode.size());
        auto Hash = Util::FinalizeDigestToString(Hasher);
        co_return co_await CreateOrOpenFile({}, Executor, Hash);
    }

    auto ShaderCache::CreateOrOpenFile(
        Co::executor_tag,
        const Ptr<Co::executor>&       Executor,
        std::span<Rhi::ShaderBytecode> Shaders) -> Co::result<MappedFileInfo*>
    {
        CryptoPP::SHA256 Hasher;
        Hasher.Update(LibraryShaderHash, std::size(LibraryShaderHash));
        for (auto& Shader : Shaders)
        {
            Hasher.Update(Shader.GetBytecode(), Shader.GetSize());
        }
        auto Hash = Util::FinalizeDigestToString(Hasher);
        co_return co_await CreateOrOpenFile({}, Executor, Hash);
    }

    auto ShaderCache::CreateOrOpenFile(
        Co::executor_tag,
        const Ptr<Co::executor>& Executor,
        const String&            FileKey) -> Co::result<MappedFileInfo*>
    {
        auto Iter = m_Cache.find(FileKey);
        if (Iter == m_Cache.end())
        {
            Co::scoped_async_lock Lock = co_await m_Mutex.lock(Executor);

            Iter = m_Cache.find(FileKey);
            if (Iter == m_Cache.end())
            {
                auto FileName = GenerateCacheFileName(m_Device, FileKey);
                Iter          = m_Cache.emplace(FileName, std::move(FileName)).first;
            }
        }
        co_return &Iter->second;
    }

    //

    Co::result<Rhi::ShaderBytecode> ShaderCache::LoadFromCache(
        Co::executor_tag,
        const Ptr<Co::executor>&      Executor,
        MappedFileInfo&               FileInfo,
        const PermutationKey&         Key,
        const Rhi::ShaderCompileDesc& Desc)
    {
        Co::scoped_async_lock Lock  = co_await FileInfo.Mutex.lock(Executor);
        CacheMap*             Cache = FileInfo.GetCache<CacheMap>();

        auto Iter = Cache->find(Key);

        Rhi::ShaderBytecode Result;
        if (Iter != Cache->end())
        {
            auto Blob     = ShaderDataHelper::ToBlob(FileInfo.File, Iter->second);
            auto ByteCode = ShaderDataHelper::ToByteCode(FileInfo.File, *Blob, Desc.GetStage());
            if (ByteCode)
            {
                Result = std::move(ByteCode);
            }
        }
        co_return Result;
    }

    //

    Co::result<Rhi::ShaderBytecode> ShaderCache::CompileAndInsertToCache(
        Co::executor_tag,
        const Ptr<Co::executor>&      Executor,
        MappedFileInfo&               FileInfo,
        StringView                    SourceCode,
        const PermutationKey&         Key,
        const Rhi::ShaderCompileDesc& Desc)
    {
        auto CompileTask = Rhi::ShaderCompiler::CompileAsync({}, *Executor, m_Device, SourceCode, Desc, &m_AssetStorage.get());
        co_return co_await InsertToCache({}, Executor, FileInfo, std::move(CompileTask), Key, Desc);
    }

    Co::result<Rhi::ShaderBytecode> ShaderCache::LinkAndInsertToCache(
        Co::executor_tag,
        const Ptr<Co::executor>&       Executor,
        MappedFileInfo&                FileInfo,
        std::span<Rhi::ShaderBytecode> Shaders,
        const PermutationKey&          Key,
        const Rhi::ShaderCompileDesc&  Desc)
    {
        auto CompileTask = Rhi::ShaderCompiler::LinkAsync({}, *Executor, m_Device, Desc, Shaders);
        co_return co_await InsertToCache({}, Executor, FileInfo, std::move(CompileTask), Key, Desc);
    }

    Co::result<Rhi::ShaderBytecode> ShaderCache::InsertToCache(
        Co::executor_tag,
        const Ptr<Co::executor>&        Executor,
        MappedFileInfo&                 FileInfo,
        Co::result<Rhi::ShaderBytecode> FinalShader,
        const PermutationKey&           Key,
        const Rhi::ShaderCompileDesc&   Desc)
    {
        Co::scoped_async_lock Lock  = co_await FileInfo.Mutex.lock(Executor);
        CacheMap*             Cache = FileInfo.GetCache<CacheMap>();

        ShaderData* Blob = nullptr;

        auto Iter = Cache->find(Key);
        if (Iter != Cache->end())
        {
            Blob = ShaderDataHelper::ToBlob(FileInfo.File, Iter->second);
            FileInfo.File.deallocate(Blob);
            Blob = nullptr;
            Cache->erase(Iter);
        }

        auto   CompiledShader = co_await FinalShader;
        size_t BlobSize       = ShaderDataHelper::CalculateBlobSize(CompiledShader);
        if (BlobSize)
        {
            for (uint32_t i = 1; i <= GrowAttempts; i++)
            {
                try
                {
                    if (!Blob)
                    {
                        Blob = ShaderDataHelper::AllocateBlob(FileInfo.File, BlobSize);
                        ShaderDataHelper::FromBytecode(FileInfo.File, *Blob, CompiledShader);
                    }
                    Cache->emplace(Key, Blob->DataHandle);
                    break;
                }
                catch (const bip::bad_alloc&)
                {
                    auto GrowSize = Cache->size() * PermutationCacheSize + i * PermutationGrowSize;
                    FileInfo.Grow(GrowSize);
                    Cache = FileInfo.GetCache<CacheMap>();
                }
            }
        }
        co_return CompiledShader;
    }

    //

    String ShaderCache::GenerateCacheFileName(
        Rhi::Device&  Device,
        const String& Hash)
    {
        return std::format("{}_{}{}.acs", std::filesystem::temp_directory_path().string(), Hash, Device.GetGraphicsAPIName());
    }

    auto ShaderCache::GeneratePermutationKey(
        const Rhi::ShaderCompileDesc& Desc) -> PermutationKey
    {
        CryptoPP::SHA256 Hasher;

        for (auto& [Key, Value] : Desc.Defines)
        {
            Hasher.Update(std::bit_cast<const CryptoPP::byte*>(Key.c_str()), sizeof(Key[0]) * Key.size());
            Hasher.Update(std::bit_cast<const CryptoPP::byte*>(Value.c_str()), sizeof(Value[0]) * Value.size());
        }
        for (auto Extension : Desc.SpirvExtensions)
        {
            Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Extension), sizeof(Extension));
        }

        auto Stage = Desc.GetStage();
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Stage), sizeof(Stage));
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Desc.Profile), sizeof(Desc.Profile));
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Desc.VulkanMemoryLayout), sizeof(Desc.VulkanMemoryLayout));
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Desc.Flags), sizeof(Desc.Flags));

        return Util::FinalizeDigest(Hasher);
    }
} // namespace Ame::Gfx::Cache
