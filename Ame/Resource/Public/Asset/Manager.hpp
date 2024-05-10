#pragma once

#include <Asset/Handle.hpp>

namespace Ame::Asset
{
    class IAsset;
    class IAssetPackage;
    class Storage;

    class Manager
    {
    public:
        Manager(
            Storage&    Storage,
            Co::runtime& Runtime) :
            m_Storage(Storage),
            m_Runtime(Runtime)
        {
        }

        /// <summary>
        /// Load asynchronously an asset from the storage system.
        /// </summary>
        Co::result<Ptr<IAsset>> LoadAsync(
            IAssetPackage* Package,
            const Handle&  AssetGuid,
            bool           LoadTemp = false);

        /// <summary>
        /// Load asynchronously an asset from the storage system.
        /// </summary>
        Co::result<Ptr<IAsset>> LoadAsync(
            const Handle& AssetGuid,
            bool          LoadTemp = false);

        /// <summary>
        /// Load an asset from the storage system.
        /// </summary>
        Ptr<IAsset> Load(
            IAssetPackage* Package,
            const Handle&  AssetGuid,
            bool           LoadTemp = false);

        /// <summary>
        /// Load an asset from the storage system.
        /// </summary>
        Ptr<IAsset> Load(
            const Handle& AssetGuid,
            bool          LoadTemp = false);

        /// <summary>
        /// Load asynchronously an asset from the storage system.
        /// </summary>
        Co::result<Ptr<IAsset>> ReloadAsync(
            const Handle& AssetGuid);

        /// <summary>
        /// Load asynchronously an asset from the storage system.
        /// </summary>
        Ptr<IAsset> Reload(
            const Handle& AssetGuid);

        /// <summary>
        /// Unload an asset from the storage system.
        /// </summary>
        /// <returns>The asset was unloaded successfully.</returns>
        bool Unload(
            const Handle& AssetGuid);

        /// <summary>
        /// Unload an asset from the storage system if it is not referenced anymore.
        /// </summary>
        /// <returns>The asset was unloaded successfully.</returns>
        bool RequestUnload(
            const Handle& AssetGuid);

    private:
        Ref<Storage>    m_Storage;
        Ref<Co::runtime> m_Runtime;
    };
} // namespace Ame::Asset