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
        Storage&              assetStorage,
        std::filesystem::path path) :
        IAssetPackage(assetStorage),
        m_RootPath(std::move(path))
    {
        auto& logger = Log::Asset();

        auto rootPathStr = m_RootPath.generic_string();
        if (rootPathStr.empty() || rootPathStr.starts_with(".."))
        {
            logger.Error("Path '{}' cannot be empty or start with '..'", rootPathStr);
            return;
        }

        if (!std::filesystem::exists(m_RootPath))
        {
            logger.Trace("Path '{}' does not exist, creating a new one", rootPathStr);
            std::filesystem::create_directories(m_RootPath);
            return;
        }

        if (!std::filesystem::is_directory(m_RootPath))
        {
            logger.Error("Trying to create a directory asset package from a file '{}'", rootPathStr);
            return;
        }

        logger.Trace("Loading assets from directory '{}'", rootPathStr);

        // First load all the meta files
        for (auto& metafilePath : GetFiles(m_RootPath))
        {
            if (!metafilePath.ends_with(AssetMetaDataDef::c_MetaFileExtension))
            {
                continue;
            }

            std::ifstream file(metafilePath);
            if (!file.is_open())
            {
                logger.Error("Failed to open meta file '{}'", metafilePath);
                continue;
            }

            {
                AssetMetaDataDef metaData(file);
                file.close();

                if ((m_RootPath / metaData.GetMetaPath()) != metafilePath)
                {
                    logger.Error("Meta file '{}' has a different path than the one in meta file", metafilePath);
                    continue;
                }

                auto guid = metaData.GetGuid();
                if (guid == Guid::c_Null)
                {
                    logger.Error("Asset", "Meta file '{}' does not have a Guid", metafilePath);
                    continue;
                }

                if (m_AssetMeta.contains(guid))
                {
                    logger.Error("Meta file '{}' has a Guid that is already in use", metafilePath);
                    continue;
                }

                auto assetPath = m_RootPath / metaData.GetAssetPath();
                file.open(assetPath, std::ios::ate | std::ios::binary);

                if (!file.is_open())
                {
                    logger.Error("Failed to open asset file '{}'", assetPath.string());
                    continue;
                }

                size_t fileSize = static_cast<size_t>(file.tellg());
                file.seekg(std::ios::beg);

                CryptoPP::SHA256 hasher;
                Util::UpdateCrypto(hasher, file, fileSize);
                auto currentHash = Util::FinalizeDigestToString(hasher);

                auto expectedHash = metaData.GetHash();
                if (expectedHash.empty())
                {
                    logger.Error("Meta file '{}' does not have a hash", metafilePath);
                    continue;
                }
                else if (currentHash != expectedHash)
                {
#ifndef AME_ASSET_MGR_DISABLE_HASH_VALIDATION
                    logger.Error("Asset file '{}' has a different hash than the one in meta file", assetPath.string());
                    continue;
#else
                    // Try to correct the hash
                    logger.Warning("Asset file '{}' has a different hash than the one in meta file", assetPath.string());
                    metaData.SetHash(currentHash);
                    metaData.SetDirty();

                    ExportMeta(metaData);
#endif
                }

                m_AssetPath.emplace(metaData.GetAssetPath().string(), guid);
                m_AssetMeta.emplace(guid, std::move(metaData));
            }
        }
    }

    Co::generator<Guid> DirectoryAssetPackage::GetAssets()
    {
        RLock readLock(m_CacheMutex);
        for (auto& guid : m_AssetMeta | std::views::keys)
        {
            co_yield Guid{ guid };
        }
    }

    bool DirectoryAssetPackage::ContainsAsset(
        const Guid& guid) const
    {
        RLock readLock(m_CacheMutex);
        return m_AssetMeta.contains(guid);
    }

    Guid DirectoryAssetPackage::FindAsset(
        const String& path) const
    {
        RLock readLock(m_CacheMutex);
        auto  iter = m_AssetPath.find(path);
        return iter != m_AssetPath.end() ? iter->second : Guid::c_Null;
    }

    Co::generator<Guid> DirectoryAssetPackage::FindAssets(
        const std::regex& pathRegex) const
    {
        RLock readLock(m_CacheMutex);
        for (auto& [path, guid] : m_AssetPath)
        {
            if (std::regex_match(path, pathRegex))
            {
                co_yield Guid{ guid };
            }
        }
    }

    Co::result<void> DirectoryAssetPackage::Export()
    {
        auto ExportTask = [this]
        {
            // Export all dirty assets
            {
                RWLock readWriteLock(m_CacheMutex);
                for (auto& [guid, metaData] : m_AssetMeta)
                {
                    if (!metaData.IsDirty())
                    {
                        continue;
                    }

                    ExportMeta(metaData);
                }
            }
        };

        auto executor = m_Runtime.get().background_executor();
        return executor->submit(std::move(ExportTask));
    }

    Co::result<void> DirectoryAssetPackage::SaveAsset(
        Ptr<IAsset> asset)
    {
        auto executor = m_Runtime.get().background_executor();
        co_await Co::resume_on(executor);
        SaveAssetAndDependencies(asset);
    }

    bool DirectoryAssetPackage::RemoveAsset(
        const Guid& guid)
    {
        RWLock readWriteLock(m_CacheMutex);

        auto iter = m_AssetMeta.find(guid);
        if (iter == m_AssetMeta.end())
        {
            return false;
        }

        m_Cache.erase(guid);
        m_AssetPath.erase(iter->second.GetMetaPath().generic_string());
        m_AssetMeta.erase(iter);
        return true;
    }

    const Guid& DirectoryAssetPackage::GetGuidOfPath(
        const String& path) const
    {
        RLock readLock(m_CacheMutex);
        auto  iter = m_AssetPath.find(path);
        return iter != m_AssetPath.end() ? iter->second : Guid::c_Null;
    }

    Ptr<IAsset> DirectoryAssetPackage::LoadAsset(
        const Guid& guid,
        bool        loadTemp)
    {
        // The reason for checking if the asset is already loaded in the cache
        // is because we later will only load the asset if it's not already loaded
        if (auto asset = LoadAssetFromCache(guid))
        {
            return asset;
        }
        return LoadAssetAndDependencies(guid, loadTemp);
    }

    bool DirectoryAssetPackage::UnloadAsset(
        const Guid& guid,
        bool        force)
    {
        RWLock readWriteLock(m_CacheMutex);

        auto iter = m_Cache.find(guid);
        if (iter == m_Cache.end())
        {
            m_Cache.erase(guid);
        }

        if (!force && iter->second->GetReferenceCounters()->GetNumStrongRefs() == 1)
        {
            m_Cache.erase(iter);
        }
        return true;
    }

    //

    Co::generator<String> DirectoryAssetPackage::GetFiles(
        const std::filesystem::path& path)
    {
        for (auto& entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (entry.is_regular_file())
            {
                co_yield entry.path().generic_string();
            }
        }
    }

    void DirectoryAssetPackage::ExportMeta(
        AssetMetaDataDef& metaData) const
    {
        std::filesystem::path metaPath = m_RootPath / metaData.GetMetaPath();
        std::filesystem::create_directories(metaPath.parent_path());

        std::ofstream metaFile(metaPath, std::ios::out | std::ios::trunc);
        metaData.Export(metaFile);
        metaData.SetDirty(false);
    }

    Ptr<IAsset> DirectoryAssetPackage::LoadAssetFromCache(
        const Guid& guid)
    {
        RLock readLock(m_CacheMutex);
        if (auto iter = m_Cache.find(guid); iter != m_Cache.end())
        {
            return iter->second;
        }
        return {};
    }

    Ptr<IAsset> DirectoryAssetPackage::LoadAssetAndDependencies(
        const Guid& guid,
        bool        loadTemp)
    {
        std::stack<Guid> toLoad;
        toLoad.push(guid);

        DependencyReader         dependencyReader;
        std::vector<Ptr<IAsset>> tempAssets;

        while (!toLoad.empty())
        {
            auto& currentGuid = toLoad.top();

            // Check if asset is already loaded in cache
            // If not, check if it exists in the package
            AssetMetaDataDef* metaData = nullptr;
            {
                RLock readLock(m_CacheMutex);
                auto  iter = m_AssetMeta.find(currentGuid);
                if (iter == m_AssetMeta.end())
                {
                    throw AssetChildMetaNotFoundException(guid, currentGuid);
                }

                metaData = &iter->second;
            }

            // If we need to load dependencies first, skip this asset and load the dependencies
            bool needsDependenciesFirst = false;

            // Insert the assets that should be loaded first
            {
                RLock readLock(m_CacheMutex);
                for (auto& depGuid : metaData->GetDependencies())
                {
                    if (auto asset = LoadAssetFromCache(depGuid))
                    {
                        dependencyReader.Link(depGuid, asset);
                    }
                    else
                    {
                        toLoad.push(std::move(depGuid));
                        needsDependenciesFirst = true;
                    }
                }
            }
            if (needsDependenciesFirst)
            {
                continue;
            }

            auto path = m_RootPath / metaData->GetAssetPath();
            if (!std::filesystem::exists(path))
            {
                throw AssetNotFoundException(guid, currentGuid);
            }

            //
            // Read asset file
            //

            IAssetHandler* handler = m_Storage.get().GetHandler(metaData->GetLoaderId());
            if (!handler)
            {
                throw toLoad.size() == 1 ? AssetWithNoHandlerException(guid) : AssetWithNoHandlerException(guid, currentGuid);
            }

            std::ifstream assetFile(path, std::ios::in | std::ios::binary);
            if (!assetFile.is_open())
            {
                throw AssetNotFoundException(guid, currentGuid);
            }

            AssetHandlerLoadDesc loadDesc{
                .BackgroundExecutor = m_Runtime.get().background_executor(),
                .ForegroundExecutor = m_Runtime.get().inline_executor(),

                .Stream       = assetFile,
                .Dependencies = dependencyReader,

                .Guid = currentGuid,
                .Path = path.string(),

                .LoaderData = metaData->GetLoaderData()
            };
            auto asset = handler->Load(loadDesc).get();
            if (!asset)
            {
                throw AssetHandlerFailureException(guid, currentGuid);
            }

            asset->MarkDirty(false);
            dependencyReader.Link(currentGuid, asset);

            if (!loadTemp) [[likely]]
            {
                RWLock readWriteLock(m_CacheMutex);
                m_Cache.emplace(currentGuid, asset);
            }

            tempAssets.emplace_back(std::move(asset));
            toLoad.pop();
        }

        return tempAssets.empty() ? Ptr<IAsset>{} : tempAssets.back();
    }

    void DirectoryAssetPackage::SaveAssetAndDependencies(
        Ptr<IAsset> firstAsset)
    {
        std::queue<Ptr<IAsset>> toSave;
        toSave.emplace(firstAsset);

        while (!toSave.empty())
        {
            auto curAsset = std::move(toSave.front());
            toSave.pop();

            auto& guid = curAsset->GetGuid();

            // Check if asset already exists in the package
            // else add it to the package
            AssetMetaDataDef* metaData = nullptr;
            {
                RWLock readWriteLock(m_CacheMutex);
                auto   iter = m_AssetMeta.find(guid);
                if (iter == m_AssetMeta.end())
                {
                    iter = m_AssetMeta.emplace(guid, AssetMetaDataDef(guid, curAsset->GetPath())).first;
                    m_AssetPath.emplace(curAsset->GetPath(), guid);
                    iter->second.SetMetaPath(std::format("{}{}", curAsset->GetPath(), AssetMetaDataDef::c_MetaFileExtension));
                }
                metaData = &iter->second;
            }

            //
            // Write asset file
            //

            auto [handler, handlerId] = m_Storage.get().GetHandler(curAsset);
            if (!handler)
            {
                Log::Asset().Error("Failed to get handler for asset '{}'", guid.ToString());
                continue;
            }

            auto assetPath = m_RootPath / metaData->GetAssetPath();
            std::filesystem::create_directories(assetPath.parent_path());

            std::fstream assetFile(assetPath, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
            if (!assetFile.is_open())
            {
                Log::Asset().Error("Failed to open asset file '{}'", assetPath.string());
                continue;
            }

            DependencyWriter dependencyWriter;

            AssetHandlerSaveDesc saveDesc{
                .BackgroundExecutor = m_Runtime.get().background_executor(),
                .ForegroundExecutor = m_Runtime.get().inline_executor(),

                .Stream       = assetFile,
                .Dependencies = dependencyWriter,

                .Asset      = curAsset,
                .LoaderData = metaData->GetLoaderData()
            };
            handler->Save(saveDesc).wait();

            // Write dependencies to metadata
            {
                std::vector<String> depsInMetaData;

                auto& dependencies = dependencyWriter.GetDependencies();
                for (auto& childAsset : dependencyWriter.GetDependencies())
                {
                    if (childAsset->IsDirty())
                    {
                        toSave.push(childAsset);
                    }
                    depsInMetaData.emplace_back(childAsset->GetGuid().ToString());
                }
                dependencies.clear();

                metaData->SetDependencies(depsInMetaData);
            }

            //
            // Write file's hash
            //

            size_t fileSize = static_cast<size_t>(assetFile.tellg());
            assetFile.seekg(std::ios::beg);

            CryptoPP::SHA256 hasher;
            Util::UpdateCrypto(hasher, assetFile, fileSize);

            metaData->SetLoaderId(handlerId);
            metaData->SetHash(Util::FinalizeDigestToString(hasher));

            curAsset->MarkDirty(false);
            metaData->SetDirty();
        }
    }
} // namespace Ame::Asset
