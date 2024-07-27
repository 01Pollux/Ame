#pragma once

#include <Asset/Core.hpp>

namespace Ame::Asset
{
    class Manager
    {
    public:
        Manager(
            Storage& assetStorage) :
            m_Storage(assetStorage)
        {
        }

        /// <summary>
        /// Load asynchronously an asset from the storage system.
        /// </summary>
        Co::result<Ptr<IAsset>> LoadAsync(
            IAssetPackage* package,
            Guid           guid,
            bool           loadTemp = false);

        /// <summary>
        /// Load asynchronously an asset from the storage system.
        /// </summary>
        Co::result<Ptr<IAsset>> LoadAsync(
            Guid guid,
            bool loadTemp = false);

        /// <summary>
        /// Load an asset from the storage system.
        /// </summary>
        Ptr<IAsset> Load(
            IAssetPackage* package,
            const Guid&    guid,
            bool           loadTemp = false);

        /// <summary>
        /// Load an asset from the storage system.
        /// </summary>
        Ptr<IAsset> Load(
            const Guid& guid,
            bool        loadTemp = false);

        /// <summary>
        /// Load asynchronously an asset from the storage system.
        /// </summary>
        Co::result<Ptr<IAsset>> ReloadAsync(
            Guid guid);

        /// <summary>
        /// Load asynchronously an asset from the storage system.
        /// </summary>
        Ptr<IAsset> Reload(
            const Guid& guid);

        /// <summary>
        /// Unload an asset from the storage system.
        /// </summary>
        /// <returns>The asset was unloaded successfully.</returns>
        bool Unload(
            const Guid& guid);

        /// <summary>
        /// Unload an asset from the storage system if it is not referenced anymore.
        /// </summary>
        /// <returns>The asset was unloaded successfully.</returns>
        bool RequestUnload(
            const Guid& guid);

    private:
        Ref<Storage> m_Storage;
    };
} // namespace Ame::Asset