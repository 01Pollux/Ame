#pragma once

#include <boost/container/flat_map.hpp>
#include <Rhi/Staging/DeferredStagingManager.hpp>

namespace Ame::Rhi::Staging
{
    class DeferredStagingManager::StateStorage
    {
    public:
        StateStorage() = default;

        StateStorage(const StateStorage&) = delete;
        StateStorage(StateStorage&&)      = delete;

        StateStorage& operator=(const StateStorage&) = delete;
        StateStorage& operator=(StateStorage&&)      = delete;

        ~StateStorage();

    public:
        /// <summary>
        /// Save states of buffers if needed
        /// </summary>
        void PersistUpload(
            CommandList&                         commandList,
            std::span<const SavedBufferCopyDesc> descs);

        /// <summary>
        /// Save states of textures if needed
        /// </summary>
        void PersistUpload(
            CommandList&                          commandList,
            std::span<const SavedTextureCopyDesc> descs);

        /// <summary>
        /// Save states of textures if need
        /// </summary>
        void PersistUpload(
            CommandList&                           commandList,
            std::span<const SavedTransferCopyDesc> descs);

        /// <summary>
        /// Save states of textures if need
        /// </summary>
        void PersistReadback(
            CommandList&                           commandList,
            std::span<const SavedTransferCopyDesc> descs);

    public:
        /// <summary>
        /// Swap back states of textures and buffers to the old states as well as clearing any pending transitions
        /// </summary>
        void Restore(
            CommandList& commandList);

    public:
        /// <summary>
        /// Commit pending transition if needed
        /// </summary>
        void Commit(
            CommandList& commandList);

    private:
        /// <summary>
        /// Persist buffer
        /// </summary>
        void PersistBuffer(
            CommandList& commandList,
            bool         persist,
            nri::Buffer* nriBuffer,
            AccessBits   accessBits);

        /// <summary>
        /// Persist texture
        /// </summary>
        void PersistTexture(
            CommandList&  commandList,
            bool          persist,
            nri::Texture* nriTexture,
            AccessBits    accessBits,
            LayoutType    layoutType);

    private:
        using BufferStateMap  = boost::container::flat_map<nri::Buffer*, AccessStage>;
        using TextureStateMap = boost::container::flat_map<nri::Texture*, std::vector<AccessLayoutStage>>;

    private:
        BufferStateMap  m_BuffersState;
        TextureStateMap m_TexturesState;
        bool            m_NeedCommit = false;
    };
} // namespace Ame::Rhi::Staging