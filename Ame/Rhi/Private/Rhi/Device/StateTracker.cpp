#include <ranges>

#include <concurrencpp/concurrencpp.h>
#include <Rhi/Device/StateTracker.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    using MipArrayLevelGenerator = Co::generator<std::pair<nri::Mip_t, nri::Dim_t>>;

    /// <summary>
    /// Helper function to iterate over all subresources in a texture.
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

    void ResourceStateTracker::RequireState(
        nri::Buffer*     buffer,
        nri::AccessStage state,
        bool             append)
    {
        Log::Rhi().Assert(m_CurrentStates.Buffers.contains(buffer), "Buffer is not being tracked");

        if (append)
        {
            auto& currentState = m_CurrentStates.Buffers[buffer];
            state.access |= currentState.access;
            state.stages |= currentState.stages;
        }

        auto& pendingStates = m_PendingStates.Buffers[buffer];
        StripUnsupportedStages(state.stages);
        pendingStates.push_back(state);
    }

    void ResourceStateTracker::RequireState(
        nri::CoreInterface&    nriCore,
        nri::Texture*          texture,
        nri::AccessLayoutStage state,
        nri::Mip_t             mipLevel,
        nri::Mip_t             mipCount,
        nri::Dim_t             arraySlice,
        nri::Dim_t             arrayCount,
        bool                   append)
    {
        Log::Rhi().Assert(m_CurrentStates.Textures.contains(texture), "Texture is not being tracked");

        if (append)
        {
            auto& currentStates = m_CurrentStates.Textures[texture];

            for (auto [Mip, Array] : ForEachSubresource(mipLevel, mipCount, arraySlice, arrayCount))
            {
                SubresourceIndex subresource(Mip, Array);

                auto& subresourceStates = currentStates[subresource];
                state.access |= subresourceStates.access;
                state.stages |= subresourceStates.stages;
            }
        }

        auto& textureDesc = nriCore.GetTextureDesc(*texture);

        if (mipCount == nri::REMAINING_MIP_LEVELS)
        {
            mipCount = textureDesc.mipNum - mipLevel;
        }
        if (arrayCount == nri::REMAINING_ARRAY_LAYERS)
        {
            arrayCount = textureDesc.arraySize - arraySlice;
        }

        StripUnsupportedStages(state.stages);

        auto& pendingStates = m_PendingStates.Textures[texture];
        for (auto [Mip, Array] : ForEachSubresource(mipLevel, mipCount, arraySlice, arrayCount))
        {
            SubresourceIndex subresource(Mip, Array);

            auto& subresourceStates = pendingStates[subresource];
            subresourceStates.push_back(state);
        }
    }

    void ResourceStateTracker::PlaceBarrier(
        const nri::GlobalBarrierDesc& barrierDesc)
    {
        m_GlobalBarriersCache.emplace_back(barrierDesc);
    }

    //

    void ResourceStateTracker::BeginTracking(
        nri::Buffer*                 buffer,
        AtomicBufferSubresourceState initialState)
    {
        AME_LOG_ASSERT(Log::Rhi(), !m_CurrentStates.Buffers.contains(buffer), "Buffer already being tracked");
        m_CurrentStates.Buffers[buffer] = initialState;
    }

    void ResourceStateTracker::BeginTracking(
        nri::CoreInterface&           nriCore,
        nri::Texture*                 texture,
        AtomicTextureSubresourceState initialState)
    {
        AME_LOG_ASSERT(Log::Rhi(), !m_CurrentStates.Textures.contains(texture), "Texture already being tracked");

        auto& textureDesc = nriCore.GetTextureDesc(*texture);

        for (auto [Mip, Array] : ForEachSubresource(0, textureDesc.mipNum, 0, textureDesc.arraySize))
        {
            SubresourceIndex subresource(Mip, Array);
            m_CurrentStates.Textures[texture][subresource] = initialState;
        }
    }

    void ResourceStateTracker::EndTracking(
        nri::Buffer* buffer)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Buffers.contains(buffer), "Buffer is not being tracked");
        m_CurrentStates.Buffers.erase(buffer);
        m_PendingStates.Buffers.erase(buffer);
    }

    void ResourceStateTracker::EndTracking(
        nri::Texture* texture)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(texture), "Texture is not being tracked");
        m_CurrentStates.Textures.erase(texture);
        m_PendingStates.Textures.erase(texture);
    }

    //

    void ResourceStateTracker::MutateState(
        nri::Buffer*                 buffer,
        AtomicBufferSubresourceState state)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Buffers.contains(buffer), "Buffer is not being tracked");
        m_CurrentStates.Buffers[buffer] = state;
    }

    void ResourceStateTracker::MutateState(
        nri::CoreInterface&           nriCore,
        nri::Texture*                 texture,
        AtomicTextureSubresourceState state,
        nri::Mip_t                    mipLevel,
        nri::Mip_t                    mipCount,
        nri::Dim_t                    arraySlice,
        nri::Dim_t                    arrayCount)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(texture), "Texture is not being tracked");

        auto& textureDesc   = nriCore.GetTextureDesc(*texture);
        auto& currentStates = m_CurrentStates.Textures[texture];

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
        if (m_PendingStates.Buffers.empty() && m_PendingStates.Textures.empty())
        {
            return;
        }

        auto buffers  = FlushBuffers();
        auto textures = FlushTextures(nriCore);

        // dont perfom call if there is no state to transition to
        if (!buffers.empty() || !textures.empty() || !m_GlobalBarriersCache.empty())
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
        for (auto& [buffer, states] : m_PendingStates.Buffers)
        {
            auto& currentState = m_CurrentStates.Buffers[buffer];

            nri::BufferBarrierDesc bufferBarrier{
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

    std::vector<nri::TextureBarrierDesc> ResourceStateTracker::FlushTextures(
        nri::CoreInterface& nriCore)
    {
        std::vector<nri::TextureBarrierDesc> textureBarriers;
        for (auto& [texture, subresourceStates] : m_PendingStates.Textures)
        {
            auto TempBarriers = TransitionTexture(nriCore, texture, subresourceStates);
            textureBarriers.insert(textureBarriers.end(), std::make_move_iterator(TempBarriers.begin()), std::make_move_iterator(TempBarriers.end()));
        }
        return textureBarriers;
    }

    std::vector<nri::TextureBarrierDesc> ResourceStateTracker::TransitionTexture(
        nri::CoreInterface&                   nriCore,
        nri::Texture*                         texture,
        const TextureSubresourceStates<true>& newStates)
    {
        auto& currentSubresources = m_CurrentStates.Textures[texture];

        bool statesMatch = true;

        auto& firstOldState = currentSubresources.begin()->second;
        auto  firstNewState = CollapseStates(newStates.begin()->second);

        std::vector<nri::TextureBarrierDesc> textureBarriers;

        for (auto& [subresourceKey, newSubresourceStates] : newStates)
        {
            auto& currentState  = currentSubresources[subresourceKey];
            auto  finalNewState = CollapseStates(newSubresourceStates);

            if (IsNewStateRedundant(currentState.access, finalNewState.access))
            {
                continue;
            }

            auto oldState = std::exchange(currentState, finalNewState);

            SubresourceIndex subresource(subresourceKey);
            textureBarriers.emplace_back(nri::TextureBarrierDesc{
                .texture     = texture,
                .before      = oldState,
                .after       = finalNewState,
                .mipOffset   = subresource.MipLevel,
                .mipNum      = 1,
                .arrayOffset = subresource.ArraySlice,
                .arraySize   = 1 });

            // If any old subresource states do not match or any of the new states do not match
            // then performing single transition barrier for all subresources is not possible
            if (AreStateEqual(oldState, firstOldState) ||
                !AreStateEqual(finalNewState, firstNewState))
            {
                statesMatch = false;
            }
        }

        // If multiple transitions were requested, but it's possible to make just one - do it
        if (statesMatch && textureBarriers.size() > 1)
        {
            textureBarriers.resize(1);

            auto& textureDesc            = nriCore.GetTextureDesc(*texture);
            textureBarriers[0].mipNum    = textureDesc.mipNum;
            textureBarriers[0].arraySize = textureDesc.arraySize;
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
        switch (m_DeviceDesc->graphicsAPI)
        {
        case nri::GraphicsAPI::D3D12:
        {
            // if (!m_DeviceDesc->areEnhancedBarriersSupported)
            {
                // Using legacy barriers, ID3D12GraphicsCommandList::ResourceBarrier does not support layout transitions
                return a.access == b.access;
            }
        }
            [[fallthrough]];
        case nri::GraphicsAPI::VULKAN:
        {
            return a.access == b.access &&
                   a.stages == b.stages;
        }
        }
        return false;
    }

    bool ResourceStateTracker::AreStateEqual(
        const nri::AccessLayoutStage& a,
        const nri::AccessLayoutStage& b) const
    {
        switch (m_DeviceDesc->graphicsAPI)
        {
        case nri::GraphicsAPI::D3D12:
        {
            // if (!m_DeviceDesc->areEnhancedBarriersSupported)
            {
                // Using legacy barriers, ID3D12GraphicsCommandList::ResourceBarrier does not support layout transitions
                return a.access == b.access;
            }
        }
            [[fallthrough]];
        case nri::GraphicsAPI::VULKAN:
        {
            return a.access == b.access &&
                   a.stages == b.stages &&
                   a.layout == b.layout;
        }
        }
        return false;
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