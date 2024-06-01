#include <Rhi/Staging/DeferredStagingManager.hpp>
#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Staging/DeferredStagingManager.StateStorage.hpp>

namespace Ame::Rhi::Staging
{
    DeferredStagingManager::StateStorage::~StateStorage() = default;

    void DeferredStagingManager::StateStorage::PersistUpload(
        CommandList&                         commandList,
        std::span<const SavedBufferCopyDesc> descs)
    {
        for (auto& desc : descs)
        {
            PersistBuffer(commandList, desc.Src.PreserveSrcState, desc.Src.NriBuffer, AccessBits::COPY_SOURCE);
            PersistBuffer(commandList, desc.Dst.PreserveSrcState, desc.Dst.NriBuffer, AccessBits::COPY_DESTINATION);
        }
    }

    void DeferredStagingManager::StateStorage::PersistUpload(
        CommandList&                          commandList,
        std::span<const SavedTextureCopyDesc> descs)
    {
        for (auto& desc : descs)
        {
            PersistTexture(commandList, desc.Src.PreserveSrcState, desc.Src.NriTexture, desc.Src.Region ? &*desc.Src.Region : nullptr, AccessBits::COPY_SOURCE, LayoutType::COPY_SOURCE);
            PersistTexture(commandList, desc.Dst.PreserveSrcState, desc.Dst.NriTexture, desc.Dst.Region ? &*desc.Dst.Region : nullptr, AccessBits::COPY_DESTINATION, LayoutType::COPY_DESTINATION);
        }
    }

    void DeferredStagingManager::StateStorage::PersistUpload(
        CommandList&                           commandList,
        std::span<const SavedTransferCopyDesc> descs)
    {
        for (auto& desc : descs)
        {
            PersistBuffer(commandList, desc.PreserveSrcState, desc.NriBuffer, AccessBits::COPY_SOURCE);
            PersistTexture(commandList, desc.PreserveDstState, desc.NriTexture, &desc.TextureRegion, AccessBits::COPY_DESTINATION, LayoutType::COPY_DESTINATION);
        }
    }

    void DeferredStagingManager::StateStorage::PersistReadback(
        CommandList&                           commandList,
        std::span<const SavedTransferCopyDesc> descs)
    {
        for (auto& desc : descs)
        {
            PersistTexture(commandList, desc.PreserveSrcState, desc.NriTexture, &desc.TextureRegion, AccessBits::COPY_SOURCE, LayoutType::COPY_SOURCE);
            PersistBuffer(commandList, desc.PreserveDstState, desc.NriBuffer, AccessBits::COPY_DESTINATION);
        }
    }

    //

    void DeferredStagingManager::StateStorage::Restore(
        CommandList& commandList)
    {
        if (!m_BuffersState.empty() &&
            !m_TexturesState.empty())
        {
            return;
        }

        for (auto& [buffer, state] : m_BuffersState)
        {
            commandList.RequireState(buffer, state);
        }

        for (auto& [texture, states] : m_TexturesState)
        {
            commandList.RequireStates(texture, states);
        }

        m_NeedCommit = true;

        m_BuffersState.clear();
        m_TexturesState.clear();
    }

    //

    void DeferredStagingManager::StateStorage::Commit(
        CommandList& commandList)
    {
        if (!m_NeedCommit)
        {
            return;
        }
        commandList.CommitBarriers();
        m_NeedCommit = false;
    }

    //

    void DeferredStagingManager::StateStorage::PersistBuffer(
        CommandList& commandList,
        bool         persist,
        nri::Buffer* nriBuffer,
        AccessBits   accessBits)
    {
        commandList.RequireState(nriBuffer, { accessBits, Rhi::StageBits::COPY });

        m_NeedCommit = true;
        if (persist && !m_BuffersState.contains(nriBuffer))
        {
            m_BuffersState.emplace(nriBuffer, commandList.QueryState(nriBuffer));
        }
    }

    void DeferredStagingManager::StateStorage::PersistTexture(
        CommandList&             commandList,
        bool                     persist,
        nri::Texture*            nriTexture,
        const TextureRegionDesc* region,
        AccessBits               accessBits,
        LayoutType               layoutType)
    {
        TextureSubresource subresource(
            region ? MipLevel(region->mipOffset, 1) : c_EntireMipChain,
            region ? ArraySlice(region->arrayOffset, 1) : c_EntireArray);
        commandList.RequireState(nriTexture, { accessBits, layoutType, Rhi::StageBits::COPY }, subresource);

        m_NeedCommit = true;
        if (persist && !m_TexturesState.contains(nriTexture))
        {
            m_TexturesState.emplace(nriTexture, GeneratorToList<std::vector>(commandList.QueryState(nriTexture)));
        }
    }
} // namespace Ame::Rhi::Staging