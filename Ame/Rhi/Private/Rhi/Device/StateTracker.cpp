#include <ranges>

#include <concurrencpp/concurrencpp.h>
#include <Rhi/Device/StateTracker.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    using MipArrayLevelGenerator = Co::generator<std::pair<nri::Mip_t, nri::Dim_t>>;

    /// <summary>
    /// Helper function to iterate over all subresources in a nriTexture.
    /// </summary>
    [[nodiscard]] static MipArrayLevelGenerator ForEachSubresource(
        nri::Mip_t mipLevel,
        nri::Mip_t mipCount,
        nri::Dim_t arraySlice,
        nri::Dim_t arrayCount)
    {
        for (uint32_t mip = mipLevel; mip < mipLevel + mipCount; mip++)
        {
            for (nri::Dim_t dim = arraySlice; dim < arraySlice + arrayCount; dim++)
            {
                co_yield std::pair(mip, dim);
            }
        }
    }

    //

    void ResourceStateTracker::Initialize(
        const nri::DeviceDesc* deviceDesc)
    {
        m_DeviceDesc = deviceDesc;
    }

    //

    auto ResourceStateTracker::QueryState(
        nri::Buffer* nriBuffer) const -> AtomicBufferSubresourceState
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Buffers.contains(nriBuffer), "Buffer is not being tracked");
        return m_CurrentStates.Buffers.at(nriBuffer);
    }

    auto ResourceStateTracker::QueryState(
        nri::Texture* nriTexture,
        nri::Mip_t    mipLevel,
        nri::Mip_t    mipCount,
        nri::Dim_t    arraySlice,
        nri::Dim_t    arrayCount) const -> Co::generator<AtomicTextureSubresourceState>
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(nriTexture), "Texture is not being tracked");

        auto& currentStates = m_CurrentStates.Textures.at(nriTexture);
        for (auto [Mip, Array] : ForEachSubresource(mipLevel, mipCount, arraySlice, arrayCount))
        {
            SubresourceIndex subresource(Mip, Array);
            co_yield AtomicTextureSubresourceState{ currentStates.at(subresource) };
        }
    }

    //

    void ResourceStateTracker::RequireState(
        nri::Buffer*     nriBuffer,
        nri::AccessStage state,
        bool             append)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Buffers.contains(nriBuffer), "Buffer is not being tracked");

        auto& pendingStates = m_PendingStates.Buffers[nriBuffer];

        StripUnsupportedStages(state.stages);

        if (!append)
        {
            pendingStates.clear();
        }
        pendingStates.push_back(state);
    }

    void ResourceStateTracker::RequireState(
        nri::CoreInterface&    nriCore,
        nri::Texture*          nriTexture,
        nri::AccessLayoutStage state,
        nri::Mip_t             mipLevel,
        nri::Mip_t             mipCount,
        nri::Dim_t             arraySlice,
        nri::Dim_t             arrayCount,
        bool                   append)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(nriTexture), "Texture is not being tracked");
        AME_LOG_ASSERT(Log::Rhi(), mipCount != 0 && arrayCount != 0, "Invalid mip or array count");

        auto& pendingStates = m_PendingStates.Textures[nriTexture];

        StripUnsupportedStages(state.stages);
        for (auto [Mip, Array] : ForEachSubresource(mipLevel, mipCount, arraySlice, arrayCount))
        {
            SubresourceIndex subresource(Mip, Array);
            auto&            subresourceStates = pendingStates[subresource];

            if (!append)
            {
                subresourceStates.clear();
            }
            subresourceStates.emplace_back(state);
        }
    }

    void ResourceStateTracker::RequireStates(
        nri::CoreInterface&                            nriCore,
        nri::Texture*                                  nriTexture,
        std::span<const AtomicTextureSubresourceState> states,
        bool                                           append)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(nriTexture), "Texture is not being tracked");

        auto& pendingStates = m_PendingStates.Textures[nriTexture];
        AME_LOG_ASSERT(Log::Rhi(), pendingStates.size() == states.size(), "Texture is not being tracked");

        for (SubresourceIndexType i = 0; i < states.size(); i++)
        {
            pendingStates[i].emplace_back(states[i]);
        }

        auto& textureDesc = nriCore.GetTextureDesc(*nriTexture);
        for (auto [Mip, Array] : ForEachSubresource(0, textureDesc.mipNum, 0, textureDesc.arraySize))
        {
            SubresourceIndex subresource(Mip, Array);
            auto&            subresourceStates = pendingStates[subresource];

            if (!append)
            {
                subresourceStates.clear();
            }
            subresourceStates.emplace_back(states[Mip + Array * textureDesc.mipNum]);
        }
    }

    void ResourceStateTracker::PlaceBarrier(
        const nri::GlobalBarrierDesc& barrierDesc)
    {
        m_GlobalBarriersCache.emplace_back(barrierDesc);
    }

    //

    void ResourceStateTracker::BeginTracking(
        nri::Buffer*                 nriBuffer,
        AtomicBufferSubresourceState initialState)
    {
        AME_LOG_ASSERT(Log::Rhi(), !m_CurrentStates.Buffers.contains(nriBuffer), "Buffer already being tracked");
        m_CurrentStates.Buffers[nriBuffer] = initialState;
    }

    void ResourceStateTracker::BeginTracking(
        nri::CoreInterface&           nriCore,
        nri::Texture*                 nriTexture,
        AtomicTextureSubresourceState initialState)
    {
        AME_LOG_ASSERT(Log::Rhi(), !m_CurrentStates.Textures.contains(nriTexture), "Texture already being tracked");

        auto& textureDesc = nriCore.GetTextureDesc(*nriTexture);

        for (auto [Mip, Array] : ForEachSubresource(0, textureDesc.mipNum, 0, textureDesc.arraySize))
        {
            SubresourceIndex subresource(Mip, Array);
            m_CurrentStates.Textures[nriTexture][subresource] = initialState;
        }
    }

    void ResourceStateTracker::EndTracking(
        nri::Buffer* nriBuffer)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Buffers.contains(nriBuffer), "Buffer is not being tracked");
        m_CurrentStates.Buffers.erase(nriBuffer);
        m_PendingStates.Buffers.erase(nriBuffer);
    }

    void ResourceStateTracker::EndTracking(
        nri::Texture* nriTexture)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(nriTexture), "Texture is not being tracked");
        m_CurrentStates.Textures.erase(nriTexture);
        m_PendingStates.Textures.erase(nriTexture);
    }

    //

    void ResourceStateTracker::MutateState(
        nri::Buffer*                 nriBuffer,
        AtomicBufferSubresourceState state)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Buffers.contains(nriBuffer), "Buffer is not being tracked");
        m_CurrentStates.Buffers[nriBuffer] = state;
    }

    void ResourceStateTracker::MutateState(
        nri::CoreInterface&           nriCore,
        nri::Texture*                 nriTexture,
        AtomicTextureSubresourceState state,
        nri::Mip_t                    mipLevel,
        nri::Mip_t                    mipCount,
        nri::Dim_t                    arraySlice,
        nri::Dim_t                    arrayCount)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(nriTexture), "Texture is not being tracked");

        auto& textureDesc   = nriCore.GetTextureDesc(*nriTexture);
        auto& currentStates = m_CurrentStates.Textures[nriTexture];

        if (mipCount == nri::REMAINING_MIP_LEVELS)
        {
            mipCount = textureDesc.mipNum - mipLevel;
        }
        if (arrayCount == nri::REMAINING_ARRAY_LAYERS)
        {
            arrayCount = textureDesc.arraySize - arraySlice;
        }

        for (auto [Mip, Array] : ForEachSubresource(mipLevel, mipCount, arraySlice, arrayCount))
        {
            SubresourceIndex subresource(Mip, Array);
            currentStates[subresource] = state;
        }
    }

    //

    void ResourceStateTracker::CommitBarriers(
        nri::CoreInterface& nriCore,
        nri::CommandBuffer& commandBuffer)
    {
        if (m_PendingStates.Buffers.empty() &&
            m_PendingStates.Textures.empty() &&
            m_GlobalBarriersCache.empty())
        {
            return;
        }

        auto buffers  = FlushBuffers();
        auto textures = FlushTextures(nriCore);

        // dont perfom call if there is no state to transition to
        if (!buffers.empty() ||
            !textures.empty() ||
            !m_GlobalBarriersCache.empty())
        {
            nri::BarrierGroupDesc barriers{
                .globals    = m_GlobalBarriersCache.data(),
                .buffers    = buffers.data(),
                .textures   = textures.data(),
                .globalNum  = static_cast<uint16_t>(m_GlobalBarriersCache.size()),
                .bufferNum  = static_cast<uint16_t>(buffers.size()),
                .textureNum = static_cast<uint16_t>(textures.size())
            };

            nriCore.CmdBarrier(commandBuffer, barriers);
        }

        m_PendingStates.Buffers.clear();
        m_PendingStates.Textures.clear();
        m_GlobalBarriersCache.clear();
    }

    //

    std::vector<nri::BufferBarrierDesc> ResourceStateTracker::FlushBuffers()
    {
        std::vector<nri::BufferBarrierDesc> bufferBarriers;
        for (auto& [nriBuffer, states] : m_PendingStates.Buffers)
        {
            auto& currentState = m_CurrentStates.Buffers[nriBuffer];

            nri::BufferBarrierDesc bufferBarrier{
                .buffer = nriBuffer,
                .before = currentState,
                .after  = CollapseStates(states)
            };

            if (AreStateEqual(bufferBarrier.before, bufferBarrier.after))
            {
                continue;
            }

            currentState = bufferBarrier.after;
            bufferBarriers.push_back(std::move(bufferBarrier));
        }
        return bufferBarriers;
    }

    //

    std::vector<nri::TextureBarrierDesc> ResourceStateTracker::FlushTextures(
        nri::CoreInterface& nriCore)
    {
        std::vector<nri::TextureBarrierDesc> textureBarriers;
        for (auto& [nriTexture, subresourceStates] : m_PendingStates.Textures)
        {
            auto TempBarriers = TransitionTexture(nriCore, nriTexture, subresourceStates);
            textureBarriers.insert(textureBarriers.end(), std::make_move_iterator(TempBarriers.begin()), std::make_move_iterator(TempBarriers.end()));
        }
        return textureBarriers;
    }

    std::vector<nri::TextureBarrierDesc> ResourceStateTracker::TransitionTexture(
        nri::CoreInterface&                   nriCore,
        nri::Texture*                         nriTexture,
        const TextureSubresourceStates<true>& newStates)
    {
        auto& currentSubresources = m_CurrentStates.Textures[nriTexture];

        std::vector<nri::TextureBarrierDesc> textureBarriers;

        for (auto newStatesIter = newStates.begin(); newStatesIter != newStates.end();)
        {
            auto& [firstSubresourceKey, firstSubresourceStates] = *newStatesIter;
            auto& firstCurrentState                             = currentSubresources[firstSubresourceKey];

            auto firstNewState = CollapseStates(firstSubresourceStates);

            // skip this state, since its redudant
            if (IsNewStateRedundant(firstCurrentState.access, firstNewState.access))
            {
                newStatesIter++;
                continue;
            }

            auto oldState = std::exchange(firstCurrentState, firstNewState);

            SubresourceIndex firstSubresource(firstSubresourceKey);

            auto& batchBarrier = textureBarriers.emplace_back(nri::TextureBarrierDesc{
                .texture     = nriTexture,
                .before      = oldState,
                .after       = firstNewState,
                .mipOffset   = firstSubresource.MipLevel,
                .mipNum      = 1,
                .arrayOffset = firstSubresource.ArrayIndex,
                .arraySize   = 1 });

            newStatesIter++;
            while (newStatesIter != newStates.end())
            {
                auto& [nextSubresourceKey, nextSubresourceStates] = *newStatesIter;
                auto& nextCurrentState                            = currentSubresources[nextSubresourceKey];

                auto nextNewState = CollapseStates(nextSubresourceStates);

                SubresourceIndex nextSubresource(nextSubresourceKey);

                // Check if the barrier can't be batched (mips aren't contiguous)
                if ((batchBarrier.arrayOffset != nextSubresource.ArrayIndex) ||
                    (batchBarrier.mipNum != nextSubresource.MipLevel))
                {
                    break;
                }

                // Don't bother batching, since both states must be equal to do so
                if (!AreStateEqual(firstNewState, nextNewState))
                {
                    break;
                }

                // stop this loop, since the next state is not redudant, and we will restart the batching on the next loop
                if (IsNewStateRedundant(nextCurrentState.access, nextNewState.access))
                {
                    break;
                }

                batchBarrier.mipNum++;
                newStatesIter++;
            }
        }

        // Cull and collapse identical transition with same mips into sized array
        auto firstBarrier = textureBarriers.begin();
        while (firstBarrier != textureBarriers.end())
        {
            auto nextBarrier = firstBarrier + 1;
            if (nextBarrier == textureBarriers.end())
            {
                break;
            }

            auto previousBarrier = firstBarrier;
            while (nextBarrier != textureBarriers.end())
            {
                if ((nextBarrier->arrayOffset == (previousBarrier->arrayOffset + 1)) &&
                    (nextBarrier->mipOffset == firstBarrier->mipOffset) &&
                    (nextBarrier->mipNum == firstBarrier->mipNum) &&
                    AreStateEqual(firstBarrier->before, nextBarrier->before) &&
                    AreStateEqual(firstBarrier->after, nextBarrier->after))
                {
                    previousBarrier = nextBarrier++;
                }
                else
                {
                    break;
                }
            }

            if (nextBarrier != firstBarrier)
            {
                auto dist               = std::distance(firstBarrier, nextBarrier);
                firstBarrier->arraySize = dist;
                firstBarrier            = textureBarriers.erase(firstBarrier + 1, nextBarrier);
            }
            else
            {
                firstBarrier++;
            }
        }

        return textureBarriers;
    }

    //

    auto ResourceStateTracker::CollapseStates(
        const AtomTextureSubresourceStateList<true>& states) -> AtomTextureSubresourceStateList<false>
    {
        AtomTextureSubresourceStateList<false> combined{};
        if (!states.empty())
        {
            combined.layout = states[0].layout;

            bool hasAllStates = false;
            for (auto& state : states)
            {
                hasAllStates |= state.stages == nri::StageBits::ALL;

                combined.access |= state.access;
                combined.stages |= state.stages;

                AME_LOG_ASSERT(Log::Rhi(), combined.layout == state.layout, "Inconsistent layout across subresources");
            }

            if (hasAllStates)
            {
                combined.stages = nri::StageBits::ALL;
            }
        }
        return combined;
    }

    auto ResourceStateTracker::CollapseStates(
        const AtomBufferSubresourceStateList<true>& states) -> AtomBufferSubresourceStateList<false>
    {
        AtomBufferSubresourceStateList<false> combined{};

        bool hasAllState = false;
        for (auto& state : states)
        {
            hasAllState |= state.stages == nri::StageBits::ALL;
            combined.access |= state.access;
            combined.stages |= state.stages;
        }

        if (hasAllState)
        {
            combined.stages = nri::StageBits::ALL;
        }
        return combined;
    }

    bool ResourceStateTracker::IsNewStateRedundant(
        nri::AccessBits current,
        nri::AccessBits next)
    {
        constexpr nri::AccessBits c_ReadOnlyStates =
            nri::AccessBits::VERTEX_BUFFER |
            nri::AccessBits::CONSTANT_BUFFER |
            nri::AccessBits::INDEX_BUFFER |
            nri::AccessBits::DEPTH_STENCIL_READ |
            nri::AccessBits::SHADER_RESOURCE |
            nri::AccessBits::ACCELERATION_STRUCTURE_READ |
            nri::AccessBits::ARGUMENT_BUFFER |
            nri::AccessBits::COPY_SOURCE;

        return (current == next) ||
               ((current & c_ReadOnlyStates) && (static_cast<uint32_t>(current & next) == static_cast<uint32_t>(next)));
    }

    //

    bool ResourceStateTracker::AreStateEqual(
        nri::AccessStage a,
        nri::AccessStage b) const
    {
        if (m_DeviceDesc->isEnchancedBarrierSupported)
        {
            return a.access == b.access &&
                   a.stages == b.stages;
        }
        else
        {
            // Using legacy barriers, ID3D12GraphicsCommandList::ResourceBarrier does not support layout transitions
            return a.access == b.access;
        }
    }

    bool ResourceStateTracker::AreStateEqual(
        const nri::AccessLayoutStage& a,
        const nri::AccessLayoutStage& b) const
    {
        if (m_DeviceDesc->isEnchancedBarrierSupported)
        {
            return a.access == b.access &&
                   a.stages == b.stages &&
                   a.layout == b.layout;
        }
        else
        {
            // Using legacy barriers, ID3D12GraphicsCommandList::ResourceBarrier does not support layout transitions
            return a.access == b.access;
        }
    }

    //

    void ResourceStateTracker::StripUnsupportedStages(
        nri::StageBits& stages)
    {
        if (!m_DeviceDesc->isMeshShaderSupported)
        {
            stages = static_cast<nri::StageBits>(static_cast<uint32_t>(stages) & ~static_cast<uint32_t>(nri::StageBits::MESH_SHADERS));
        }

        if (!m_DeviceDesc->isRayTracingSupported)
        {
            stages = static_cast<nri::StageBits>(static_cast<uint32_t>(stages) & ~static_cast<uint32_t>(nri::StageBits::RAY_TRACING_SHADERS));
        }
    }
} // namespace Ame::Rhi