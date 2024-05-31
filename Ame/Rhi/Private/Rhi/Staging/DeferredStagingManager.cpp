#include <Rhi/Staging/DeferredStagingManager.hpp>
#include <Rhi/Staging/DeferredStagingManager.StateStorage.hpp>
#include <Rhi/CommandList/CommandList.hpp>

namespace Ame::Rhi::Staging
{
    DeferredStagingManager::DeferredStagingManager(
        Device& rhiDevice) :
        m_Device(rhiDevice),
        m_StateStorage(std::make_unique<StateStorage>())
    {
    }

    DeferredStagingManager::~DeferredStagingManager() = default;

    //

    void DeferredStagingManager::QueueBufferCopy(
        const DeferredBufferCopyDesc& src,
        const DeferredBufferCopyDesc& dst,
        size_t                        size)
    {
        m_BufferCopies.emplace_back(src, dst, size);
    }

    void DeferredStagingManager::QueueTextureCopy(
        const DeferredTextureCopyDesc& src,
        const DeferredTextureCopyDesc& dst)
    {
        m_TextureCopies.emplace_back(src, dst);
    }

    void DeferredStagingManager::QueueUpload(
        const DeferredTransferCopyDesc& copyDesc)
    {
        m_UploadCopies.emplace_back(copyDesc);
    }

    void DeferredStagingManager::QueueReadback(
        const DeferredTransferCopyDesc& copyDesc)
    {
        m_ReadbackCopies.emplace_back(copyDesc);
    }

    //

    void DeferredStagingManager::Flush()
    {
        bool hasCopies =
            !m_BufferCopies.empty() ||
            !m_TextureCopies.empty() ||
            !m_UploadCopies.empty() ||
            !m_ReadbackCopies.empty();

        if (!hasCopies)
        {
            return;
        }

        CommandList commandList(m_Device);

        PrepareForCopy(commandList);
        DoCopy(commandList);
        RestoreFromCopy(commandList);
    }

    void DeferredStagingManager::PrepareForCopy(
        CommandList& commandList)
    {
        m_StateStorage->PersistUpload(commandList, m_BufferCopies);
        m_StateStorage->PersistUpload(commandList, m_TextureCopies);
        m_StateStorage->PersistUpload(commandList, m_UploadCopies);
        m_StateStorage->PersistReadback(commandList, m_ReadbackCopies);
        m_StateStorage->Commit(commandList);
    }

    void DeferredStagingManager::DoCopy(
        CommandList& commandList)
    {
        for (auto& desc : m_BufferCopies)
        {
            commandList.CopyBuffer(desc.Src, desc.Dst, desc.Size);
        }
        for (auto& desc : m_TextureCopies)
        {
            commandList.CopyTexture(desc.Src, desc.Dst);
        }
        for (auto& desc : m_UploadCopies)
        {
            commandList.UploadTexture(desc);
        }
        for (auto& desc : m_ReadbackCopies)
        {
            commandList.ReadbackTexture(desc);
        }

        m_BufferCopies.clear();
        m_TextureCopies.clear();
        m_UploadCopies.clear();
        m_ReadbackCopies.clear();
    }

    void DeferredStagingManager::RestoreFromCopy(
        CommandList& commandList)
    {
        m_StateStorage->Restore(commandList);
        m_StateStorage->Commit(commandList);
    }
} // namespace Ame::Rhi::Staging