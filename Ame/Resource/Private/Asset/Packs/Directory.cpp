#include <queue>
#include <stack>
#include <fstream>
#include <regex>

#include <Asset/Packs/Directory.hpp>
#include <Asset/Storage.hpp>
#include <Asset/Handler.hpp>
#include <Asset/Exception.hpp>

#include <Util/Crypto.hpp>
#include <cryptopp/sha.h>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    DirectoryAssetPackage::DirectoryAssetPackage(
        Storage&              Storage,
        std::filesystem::path Path) :
        IAssetPackage(Storage),
        m_RootPath(std::move(Path))
    {
        auto& Logger = Log::Asset();

        auto RootPathStr = m_RootPath.generic_string();
        if (RootPathStr.empty() || RootPathStr.starts_with(".."))
        {
            Logger.Error("Path '{}' cannot be empty or start with '..'", RootPathStr);
            return;
        }

        if (!std::filesystem::exists(m_RootPath))
        {
            Logger.Trace("Path '{}' does not exist, creating a new one", RootPathStr);
            std::filesystem::create_directories(m_RootPath);
            return;
        }

        if (!std::filesystem::is_directory(m_RootPath))
        {
            Logger.Error("Trying to create a directory asset package from a file '{}'", RootPathStr);
            return;
        }

        Logger.Trace("Loading assets from directory '{}'", RootPathStr);

        // First load all the meta files
        for (auto& MetafilePath : GetFiles(m_RootPath))
        {
            if (!MetafilePath.ends_with(AssetMetaDataDef::s_MetaFileExtension))
            {
                continue;
            }

            std::ifstream File(MetafilePath);
            if (!File.is_open())
            {
                Logger.Error("Failed to open meta file '{}'", MetafilePath);
                continue;
            }

            {
                AssetMetaDataDef Metadata(File);
                File.close();

                if ((m_RootPath / Metadata.GetMetaPath()) != MetafilePath)
                {
                    Logger.Error("Meta file '{}' has a different path than the one in meta file", MetafilePath);
                    continue;
                }

                auto Guid = Metadata.GetGuid();
                if (Guid == Handle::Null)
                {
                    Logger.Error("Asset", "Meta file '{}' does not have a Guid", MetafilePath);
                    continue;
                }

                if (m_AssetMeta.contains(Guid))
                {
                    Logger.Error("Meta file '{}' has a Guid that is already in use", MetafilePath);
                    continue;
                }

                auto AssetFile = m_RootPath / Metadata.GetAssetPath();
                File.open(AssetFile, std::ios::ate | std::ios::binary);

                if (!File.is_open())
                {
                    Logger.Error("Failed to open asset file '{}'", AssetFile.string());
                    continue;
                }

                size_t FileSize = static_cast<size_t>(File.tellg());
                File.seekg(std::ios::beg);

                CryptoPP::SHA256 Hasher;
                Util::UpdateCrypto(Hasher, File, FileSize);
                auto CurrentHash = Util::FinalizeDigestToString(Hasher);

                auto ExpectedHash = Metadata.GetHash();
                if (ExpectedHash.empty())
                {
                    Logger.Error("Meta file '{}' does not have a hash", MetafilePath);
                    continue;
                }
                else if (CurrentHash != ExpectedHash)
                {
#ifndef AME_ASSET_MGR_DISABLE_HASH_VALIDATION
                    Logger.Error("Asset file '{}' has a different hash than the one in meta file", AssetFile.string());
                    continue;
#else
                    // Try to correct the hash
                    Logger.Warning("Asset file '{}' has a different hash than the one in meta file", AssetFile.string());
                    Metadata.SetHash(CurrentHash);
                    Metadata.SetDirty();

                    ExportMeta(Metadata);
#endif
                }

                m_AssetPath.emplace(Metadata.GetAssetPath().string(), Guid);
                m_AssetMeta.emplace(Guid, std::move(Metadata));
            }
        }
    }

    Co::generator<const Asset::Handle&> DirectoryAssetPackage::GetAssets()
    {
        RLock Lock(m_CacheMutex);
        for (auto& Guid : m_AssetMeta | std::views::keys)
        {
            co_yield Guid;
        }
    }

    bool DirectoryAssetPackage::ContainsAsset(
        const Asset::Handle& AssetGuid) const
    {
        RLock Lock(m_CacheMutex);
        return m_AssetMeta.contains(AssetGuid);
    }

    Asset::Handle DirectoryAssetPackage::FindAsset(
        const String& Path) const
    {
        RLock Lock(m_CacheMutex);
        auto  Iter = m_AssetPath.find(Path);
        return Iter != m_AssetPath.end() ? Iter->second : Asset::Handle::Null;
    }

    Co::generator<const Asset::Handle&> DirectoryAssetPackage::FindAssets(
        const std::regex& PathRegex) const
    {
        RLock Lock(m_CacheMutex);
        for (auto& [Path, Handle] : m_AssetPath)
        {
            if (std::regex_match(Path, PathRegex))
            {
                co_yield Asset::Handle{ Handle };
            }
        }
    }

    Co::result<void> DirectoryAssetPackage::Export()
    {
        auto ExportTask = [this]
        {
            // Export all dirty assets
            {
                RWLock Lock(m_CacheMutex);
                for (auto& [Guid, Metadata] : m_AssetMeta)
                {
                    if (!Metadata.IsDirty())
                    {
                        continue;
                    }

                    ExportMeta(Metadata);
                }
            }
        };

        auto Executor = m_Runtime.get().background_executor();
        return Executor->submit(std::move(ExportTask));
    }

    Co::result<void> DirectoryAssetPackage::SaveAsset(
        Ptr<IAsset> Asset)
    {
        auto Executor = m_Runtime.get().background_executor();
        co_await Co::resume_on(Executor);
        SaveAssetAndDependencies(Asset);
    }

    bool DirectoryAssetPackage::RemoveAsset(
        const Asset::Handle& AssetGuid)
    {
        RWLock Lock(m_CacheMutex);

        auto Iter = m_AssetMeta.find(AssetGuid);
        if (Iter == m_AssetMeta.end())
        {
            return false;
        }

        m_Cache.erase(AssetGuid);
        m_AssetPath.erase(Iter->second.GetMetaPath().generic_string());
        m_AssetMeta.erase(Iter);
        return true;
    }

    const Asset::Handle& DirectoryAssetPackage::GetGuidOfPath(
        const String& Path) const
    {
        RLock Lock(m_CacheMutex);
        auto  Iter = m_AssetPath.find(Path);
        return Iter != m_AssetPath.end() ? Iter->second : Asset::Handle::Null;
    }

    Ptr<IAsset> DirectoryAssetPackage::LoadAsset(
        const Asset::Handle& AssetGuid,
        bool                 LoadTemp)
    {
        // The reason for checking if the asset is already loaded in the cache
        // is because we later will only load the asset if it's not already loaded
        if (auto CacheAsset = LoadAssetFromCache(AssetGuid))
        {
            return CacheAsset;
        }
        return LoadAssetAndDependencies(AssetGuid, LoadTemp);
    }

    bool DirectoryAssetPackage::UnloadAsset(
        const Asset::Handle& AssetGuid,
        bool                 Force)
    {
        RWLock Lock(m_CacheMutex);

        auto Iter = m_Cache.find(AssetGuid);
        if (Iter == m_Cache.end())
        {
            m_Cache.erase(AssetGuid);
        }

        if (!Force && Iter->second.use_count() == 1)
        {
            m_Cache.erase(Iter);
        }
        return true;
    }

    //

    Co::generator<String> DirectoryAssetPackage::GetFiles(
        const std::filesystem::path& Path)
    {
        for (auto& Entry : std::filesystem::recursive_directory_iterator(Path))
        {
            if (Entry.is_regular_file())
            {
                co_yield Entry.path().generic_string();
            }
        }
    }

    void DirectoryAssetPackage::ExportMeta(
        AssetMetaDataDef& Meta) const
    {
        std::filesystem::path MetaPath = m_RootPath / Meta.GetMetaPath();
        std::filesystem::create_directories(MetaPath.parent_path());

        std::ofstream Metafile(MetaPath, std::ios::out | std::ios::trunc);
        Meta.Export(Metafile);
        Meta.SetDirty(false);
    }

    Ptr<IAsset> DirectoryAssetPackage::LoadAssetFromCache(
        const Asset::Handle& AssetGuid)
    {
        RLock Lock(m_CacheMutex);
        if (auto Iter = m_Cache.find(AssetGuid); Iter != m_Cache.end())
        {
            return Iter->second;
        }
        return nullptr;
    }

    Ptr<IAsset> DirectoryAssetPackage::LoadAssetAndDependencies(
        const Asset::Handle& AssetGuid,
        bool                 LoadTemp)
    {
        std::stack<Handle> ToLoad;
        ToLoad.push(AssetGuid);

        Asset::DependencyReader  DepReader;
        std::vector<Ptr<IAsset>> TempAssets;

        while (!ToLoad.empty())
        {
            auto& CurrentGuid = ToLoad.top();

            // Check if asset is already loaded in cache
            // If not, check if it exists in the package
            AssetMetaDataDef* Metadata = nullptr;
            {
                RLock Lock(m_CacheMutex);
                auto  Iter = m_AssetMeta.find(CurrentGuid);
                if (Iter == m_AssetMeta.end())
                {
                    throw AssetChildMetaNotFoundException(AssetGuid, CurrentGuid);
                }

                Metadata = &Iter->second;
            }

            // If we need to load dependencies first, skip this asset and load the dependencies
            bool NeedsDependenciesFirst = false;

            // Insert the assets that should be loaded first
            {
                RLock Lock(m_CacheMutex);
                for (auto& DepGuid : Metadata->GetDependencies())
                {
                    if (auto CacheAsset = LoadAssetFromCache(DepGuid))
                    {
                        DepReader.Link(DepGuid, CacheAsset);
                    }
                    else
                    {
                        ToLoad.push(std::move(DepGuid));
                        NeedsDependenciesFirst = true;
                    }
                }
            }
            if (NeedsDependenciesFirst)
            {
                continue;
            }

            auto AssetPath = m_RootPath / Metadata->GetAssetPath();
            if (!std::filesystem::exists(AssetPath))
            {
                throw AssetNotFoundException(AssetGuid, CurrentGuid);
            }

            //
            // Read asset file
            //

            IAssetHandler* Handler = m_Storage.get().GetHandler(Metadata->GetLoaderId());
            if (!Handler)
            {
                throw AssetWithNoHandlerException(AssetGuid, CurrentGuid);
            }

            std::ifstream AssetFile(AssetPath, std::ios::in | std::ios::binary);
            if (!AssetFile.is_open())
            {
                throw AssetNotFoundException(AssetGuid, CurrentGuid);
            }

            auto Asset = Handler->Load(AssetFile, DepReader, CurrentGuid, AssetPath.string(), Metadata->GetLoaderData());
            if (!Asset)
            {
                throw AssetHandlerFailureException(AssetGuid, CurrentGuid);
            }

            Asset->MarkDirty(false);
            DepReader.Link(CurrentGuid, Asset);

            if (!LoadTemp) [[likely]]
            {
                RWLock Lock(m_CacheMutex);
                m_Cache.emplace(CurrentGuid, std::move(Asset));
            }

            TempAssets.emplace_back(std::move(Asset));
            ToLoad.pop();
        }

        return TempAssets.empty() ? nullptr : TempAssets.back();
    }

    void DirectoryAssetPackage::SaveAssetAndDependencies(
        Ptr<IAsset> FirstAsset)
    {
        std::queue<Ptr<IAsset>> ToSave;
        ToSave.emplace(FirstAsset);

        while (!ToSave.empty())
        {
            auto CurrentAsset = std::move(ToSave.front());
            ToSave.pop();

            auto& AssetGuid = CurrentAsset->GetGuid();

            // Check if asset already exists in the package
            // else add it to the package
            AssetMetaDataDef* Metadata = nullptr;
            {
                RWLock Lock(m_CacheMutex);
                auto   Iter = m_AssetMeta.find(AssetGuid);
                if (Iter == m_AssetMeta.end())
                {
                    Iter = m_AssetMeta.emplace(AssetGuid, AssetMetaDataDef(AssetGuid, CurrentAsset->GetPath())).first;
                    m_AssetPath.emplace(CurrentAsset->GetPath(), AssetGuid);
                    Iter->second.SetMetaPath(std::format("{}{}", CurrentAsset->GetPath(), AssetMetaDataDef::s_MetaFileExtension));
                }
                Metadata = &Iter->second;
            }

            //
            // Write asset file
            //

            size_t         HandlerId;
            IAssetHandler* Handler = m_Storage.get().GetHandler(CurrentAsset, &HandlerId);
            if (!Handler)
            {
                Log::Asset().Error("Failed to get handler for asset '{}'", AssetGuid.ToString());
                continue;
            }

            auto AssetPath = m_RootPath / Metadata->GetAssetPath();
            std::filesystem::create_directories(AssetPath.parent_path());

            std::fstream AssetFile(AssetPath, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
            if (!AssetFile.is_open())
            {
                Log::Asset().Error("Failed to open asset file '{}'", AssetPath.string());
                continue;
            }

            DependencyWriter DepWriter;
            Handler->Save(AssetFile, DepWriter, CurrentAsset, Metadata->GetLoaderData());

            // Write dependencies to metadata
            {
                std::vector<String> DepsInsertInMetadata;

                auto& Dependencies = DepWriter.GetDependencies();
                for (auto& Dep : DepWriter.GetDependencies())
                {
                    if (Dep->IsDirty())
                    {
                        ToSave.push(Dep);
                    }
                    DepsInsertInMetadata.emplace_back(Dep->GetGuid().ToString());
                }
                Dependencies.clear();

                Metadata->SetDependencies(DepsInsertInMetadata);
            }

            //
            // Write file's hash
            //

            size_t FileSize = static_cast<size_t>(AssetFile.tellg());
            AssetFile.seekg(std::ios::beg);

            CryptoPP::SHA256 Hasher;
            Util::UpdateCrypto(Hasher, AssetFile, FileSize);

            Metadata->SetLoaderId(HandlerId);
            Metadata->SetHash(Util::FinalizeDigestToString(Hasher));

            CurrentAsset->MarkDirty(false);
            Metadata->SetDirty();
        }
    }
} // namespace Ame::Asset
