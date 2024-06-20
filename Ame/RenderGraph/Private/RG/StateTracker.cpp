#include <ranges>

#include <concurrencpp/concurrencpp.h>
#include <RG/StateTracker.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    using MipArrayLevelGenerator = Co::generator<std::pair<Rhi::Mip_t, Rhi::Dim_t>>;

    /// <summary>
    /// Helper function to iterate over all subresources in a texture.
    /// </summary>
    [[nodiscard]] static MipArrayLevelGenerator ForEachSubresource(
        Rhi::Mip_t mipLevel,
        Rhi::Mip_t mipCount,
        Rhi::Dim_t arraySlice,
        Rhi::Dim_t arrayCount)
    {
        for (uint32_t mip = mipLevel; mip < mipLevel + mipCount; mip++)
        {
            for (Rhi::Dim_t dim = arraySlice; dim < arraySlice + arrayCount; dim++)
            {
                co_yield std::pair(mip, dim);
            }
        }
    }

    /// <summary>
    /// Helper function to iterate over all subresources in a texture.
    /// </summary>
    [[nodiscard]] static MipArrayLevelGenerator ForEachSubresource(
        const Rhi::TextureSubresource& subresource)
    {
        return ForEachSubresource(
            subresource.Mips.Offset,
            subresource.Mips.Count,
            subresource.Array.Offset,
            subresource.Array.Count);
    }

    //

    ResourceStateTracker::ResourceStateTracker(
        const Rhi::DeviceDesc& deviceDesc) :
        m_DeviceDesc(deviceDesc)
    {
    }

    //

    auto ResourceStateTracker::QueryState(
        const Rhi::Buffer& buffer) const -> AtomicBufferSubresourceState
    {
        auto nriBuffer = buffer.Unwrap();
        AME_LOG_ASSERT(Log::Gfx(), m_CurrentStates.Buffers.contains(nriBuffer), "Buffer is not being tracked");

        return m_CurrentStates.Buffers.at(nriBuffer);
    }

    auto ResourceStateTracker::QueryState(
        const Rhi::Texture&            texture,
        const Rhi::TextureSubresource& subresource) const -> Co::generator<AtomicTextureSubresourceState>
    {
        auto nriTexture = texture.Unwrap();
        AME_LOG_ASSERT(Log::Gfx(), m_CurrentStates.Textures.contains(nriTexture), "Texture is not being tracked");

        auto& currentStates          = m_CurrentStates.Textures.at(nriTexture);
        auto  transformedSubresource = subresource.Transform(texture);

        for (auto [mip, arr] : ForEachSubresource(transformedSubresource))
        {
            SubresourceIndex subresource(mip, arr);
            co_yield AtomicTextureSubresourceState{ currentStates.at(subresource) };
        }
    }

    //

    void ResourceStateTracker::RequireState(
        const Rhi::Buffer& buffer,
        Rhi::AccessStage   state,
        bool               append)
    {
        auto nriBuffer = buffer.Unwrap();
        AME_LOG_ASSERT(Log::Gfx(), m_CurrentStates.Buffers.contains(nriBuffer), "Buffer is not being tracked");

        auto& pendingStates = m_PendingStates.Buffers[nriBuffer];
        StripUnsupportedStages(state.stages);

        if (!append)
        {
            pendingStates.clear();
        }
        pendingStates.push_back(state);
    }

    void ResourceStateTracker::RequireState(
        const Rhi::Texture&            texture,
        AtomicTextureSubresourceState  state,
        const Rhi::TextureSubresource& subresource,
        bool                           append)
    {
        auto nriTexture = texture.Unwrap();
        AME_LOG_ASSERT(Log::Gfx(), m_CurrentStates.Textures.contains(nriTexture), "Texture is not being tracked");

        auto& pendingStates          = m_PendingStates.Textures[nriTexture];
        auto  transformedSubresource = subresource.Transform(texture);
        StripUnsupportedStages(state.stages);

        for (auto [mip, arr] : ForEachSubresource(transformedSubresource))
        {
            SubresourceIndex subresource(mip, arr);
            auto&            subresourceStates = pendingStates[subresource];

            if (!append)
            {
                subresourceStates.clear();
            }
            subresourceStates.emplace_back(state);
        }
    }

    void ResourceStateTracker::RequireStates(
        const Rhi::Texture&                            texture,
        std::span<const AtomicTextureSubresourceState> states,
        bool                                           append)
    {
        auto nriTexture  = texture.Unwrap();
        auto textureIter = m_CurrentStates.Textures.find(nriTexture);

        AME_LOG_ASSERT(Log::Gfx(), textureIter != m_CurrentStates.Textures.end(), "Texture is not being tracked");
        AME_LOG_ASSERT(Log::Gfx(), textureIter->second.size() == states.size(), "Invalid number of states");

        auto& pendingStates = m_PendingStates.Textures[nriTexture];
        auto& textureDesc   = texture.GetDesc();

        for (auto [mip, arr] : ForEachSubresource(0, textureDesc.mipNum, 0, textureDesc.arraySize))
        {
            SubresourceIndex subresource(mip, arr);
            auto&            subresourceStates = pendingStates[subresource];

            if (!append)
            {
                subresourceStates.clear();
            }

            auto state = states[mip + arr * textureDesc.mipNum];
            StripUnsupportedStages(state.stages);

            subresourceStates.emplace_back(state);
        }
    }

    void ResourceStateTracker::PlaceBarrier(
        const Rhi::GlobalBarrierDesc& barrierDesc)
    {
        m_GlobalBarriersCache.emplace_back(barrierDesc);
    }

    //

    void ResourceStateTracker::BeginTracking(
        const Rhi::Buffer&           buffer,
        AtomicBufferSubresourceState initialState)
    {
        EndTracking(buffer);

        auto nriBuffer = buffer.Unwrap();
        AME_LOG_ASSERT(Log::Gfx(), !m_CurrentStates.Buffers.contains(nriBuffer), "Buffer already being tracked");

        m_CurrentStates.Buffers[nriBuffer] = initialState;
    }

    void ResourceStateTracker::BeginTracking(
        const Rhi::Texture&           texture,
        AtomicTextureSubresourceState initialState)
    {
        EndTracking(texture);

        auto nriTexture = texture.Unwrap();
        AME_LOG_ASSERT(Log::Gfx(), !m_CurrentStates.Textures.contains(nriTexture), "Texture already being tracked");

        auto& textureDesc = texture.GetDesc();
        for (auto [mip, arr] : ForEachSubresource(0, textureDesc.mipNum, 0, textureDesc.arraySize))
        {
            SubresourceIndex subresource(mip, arr);
            m_CurrentStates.Textures[nriTexture][subresource] = initialState;
        }
    }

    void ResourceStateTracker::EndTracking(
        const Rhi::Buffer& buffer)
    {
        auto nriBuffer = buffer.Unwrap();
        m_CurrentStates.Buffers.erase(nriBuffer);
        m_PendingStates.Buffers.erase(nriBuffer);
    }

    void ResourceStateTracker::EndTracking(
        const Rhi::Texture& texture)
    {
        auto nriTexture = texture.Unwrap();
        m_CurrentStates.Textures.erase(nriTexture);
        m_PendingStates.Textures.erase(nriTexture);
    }

    //

    void ResourceStateTracker::CommitBarriers(
        Rhi::CommandList& commandList)
    {
        if (m_PendingStates.Buffers.empty() &&
            m_PendingStates.Textures.empty() &&
            m_GlobalBarriersCache.empty())
        {
            return;
        }

        auto buffers  = FlushBuffers();
        auto textures = FlushTextures();

        // dont perfom call if there is no state to transition to
        if (!buffers.empty() ||
            !textures.empty() ||
            !m_GlobalBarriersCache.empty())
        {
            Rhi::BarrierGroupDesc barriers{
                .globals    = m_GlobalBarriersCache.data(),
                .buffers    = buffers.data(),
                .textures   = textures.data(),
                .globalNum  = static_cast<uint16_t>(m_GlobalBarriersCache.size()),
                .bufferNum  = static_cast<uint16_t>(buffers.size()),
                .textureNum = static_cast<uint16_t>(textures.size())
            };
            commandList.ResourceBarrier(barriers);
        }

        m_PendingStates.Buffers.clear();
        m_PendingStates.Textures.clear();
        m_GlobalBarriersCache.clear();
    }

    //

    std::vector<Rhi::BufferBarrierDesc> ResourceStateTracker::FlushBuffers()
    {
        std::vector<Rhi::BufferBarrierDesc> bufferBarriers;
        for (auto& [buffer, states] : m_PendingStates.Buffers)
        {
            auto& currentState = m_CurrentStates.Buffers[buffer];

            Rhi::BufferBarrierDesc bufferBarrier{
                .buffer = buffer,
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

    std::vector<Rhi::TextureBarrierDesc> ResourceStateTracker::FlushTextures()
    {
        std::vector<Rhi::TextureBarrierDesc> textureBarriers;
        for (auto& [texture, subresourceStates] : m_PendingStates.Textures)
        {
            auto TempBarriers = TransitionTexture(texture, subresourceStates);
            textureBarriers.insert(textureBarriers.end(), std::make_move_iterator(TempBarriers.begin()), std::make_move_iterator(TempBarriers.end()));
        }
        return textureBarriers;
    }

    std::vector<Rhi::TextureBarrierDesc> ResourceStateTracker::TransitionTexture(
        nri::Texture*                         nriTexture,
        const TextureSubresourceStates<true>& newStates)
    {
        auto& currentSubresources = m_CurrentStates.Textures[nriTexture];

        std::vector<Rhi::TextureBarrierDesc> textureBarriers;

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

            auto& batchBarrier = textureBarriers.emplace_back(Rhi::TextureBarrierDesc{
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
                hasAllStates |= state.stages == Rhi::StageBits::ALL;

                combined.access |= state.access;
                combined.stages |= state.stages;

                AME_LOG_ASSERT(Log::Gfx(), combined.layout == state.layout, "Inconsistent layout across subresources");
            }

            if (hasAllStates)
            {
                combined.stages = Rhi::StageBits::ALL;
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
            hasAllState |= state.stages == Rhi::StageBits::ALL;
            combined.access |= state.access;
            combined.stages |= state.stages;
        }

        if (hasAllState)
        {
            combined.stages = Rhi::StageBits::ALL;
        }
        return combined;
    }

    bool ResourceStateTracker::IsNewStateRedundant(
        Rhi::AccessBits current,
        Rhi::AccessBits next)
    {
        constexpr Rhi::AccessBits c_ReadOnlyStates =
            Rhi::AccessBits::VERTEX_BUFFER |
            Rhi::AccessBits::CONSTANT_BUFFER |
            Rhi::AccessBits::INDEX_BUFFER |
            Rhi::AccessBits::DEPTH_STENCIL_READ |
            Rhi::AccessBits::SHADER_RESOURCE |
            Rhi::AccessBits::ACCELERATION_STRUCTURE_READ |
            Rhi::AccessBits::ARGUMENT_BUFFER |
            Rhi::AccessBits::COPY_SOURCE;

        return (current == next) ||
               ((current & c_ReadOnlyStates) && (static_cast<uint32_t>(current & next) == static_cast<uint32_t>(next)));
    }

    //

    bool ResourceStateTracker::AreStateEqual(
        Rhi::AccessStage a,
        Rhi::AccessStage b) const
    {
        if (m_DeviceDesc.get().isEnchancedBarrierSupported)
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
        const Rhi::AccessLayoutStage& a,
        const Rhi::AccessLayoutStage& b) const
    {
        if (m_DeviceDesc.get().isEnchancedBarrierSupported)
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
        Rhi::StageBits& stages)
    {
        if (!m_DeviceDesc.get().isMeshShaderSupported)
        {
            stages = static_cast<Rhi::StageBits>(static_cast<uint32_t>(stages) & ~static_cast<uint32_t>(Rhi::StageBits::MESH_SHADERS));
        }

        if (!m_DeviceDesc.get().isRayTracingSupported)
        {
            stages = static_cast<Rhi::StageBits>(static_cast<uint32_t>(stages) & ~static_cast<uint32_t>(Rhi::StageBits::RAY_TRACING_SHADERS));
        }
    }
} // namespace Ame::RG