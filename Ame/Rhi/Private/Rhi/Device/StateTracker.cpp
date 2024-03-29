#include "StateTracker.hpp"
#include <concurrencpp/concurrencpp.h>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    using MipArrayLevelGenerator = Co::generator<std::pair<nri::Mip_t, nri::Dim_t>>;

    /// <summary>
    /// Helper function to iterate over all subresources in a texture.
    /// </summary>
    [[nodiscard]] static MipArrayLevelGenerator ForEachSubresource(
        nri::Mip_t MipLevel,
        nri::Mip_t MipCount,
        nri::Dim_t ArraySlice,
        nri::Dim_t ArrayCount)
    {
        for (nri::Mip_t i = MipLevel; i < MipLevel + MipCount; i++)
        {
            for (nri::Dim_t j = ArraySlice; j < ArraySlice + ArrayCount; j++)
            {
                co_yield std::pair(i, j);
            }
        }
    }

    //

    void ResourceStateTracker::RequireState(
        nri::Buffer*     Buffer,
        nri::AccessStage State,
        bool             Append)
    {
        if (Append)
        {
            auto& CurrentState = m_CurrentStates.Buffers[Buffer];
            State.access |= CurrentState.access;
            State.stages |= CurrentState.stages;
        }

        auto& PendingStates = m_PendingStates.Buffers[Buffer];
        PendingStates.push_back(State);
    }

    void ResourceStateTracker::RequireState(
        nri::CoreInterface&    NriCore,
        nri::Texture*          Texture,
        nri::AccessLayoutStage State,
        nri::Mip_t             MipLevel,
        nri::Mip_t             MipCount,
        nri::Dim_t             ArraySlice,
        nri::Dim_t             ArrayCount,
        bool                   Append)
    {
        if (Append)
        {
            auto& CurrentStates = m_CurrentStates.Textures[Texture];

            for (auto [Mip, Array] : ForEachSubresource(MipLevel, MipCount, ArraySlice, ArrayCount))
            {
                SubresourceIndex Index(Mip, Array);

                auto& SubresourceStates = CurrentStates[Index];
                State.access |= SubresourceStates.access;
                State.stages |= SubresourceStates.stages;
            }
        }

        auto& Desc = NriCore.GetTextureDesc(*Texture);

        if (MipCount == nri::REMAINING_MIP_LEVELS)
        {
            MipCount = Desc.mipNum - MipLevel;
        }
        if (ArrayCount == nri::REMAINING_ARRAY_LAYERS)
        {
            ArrayCount = Desc.arraySize - ArraySlice;
        }

        auto& PendingStates = m_PendingStates.Textures[Texture];
        for (auto [Mip, Array] : ForEachSubresource(MipLevel, MipCount, ArraySlice, ArrayCount))
        {
            SubresourceIndex Index(Mip, Array);

            auto& SubresourceStates = PendingStates[Index];
            SubresourceStates.push_back(State);
        }
    }

    void ResourceStateTracker::PlaceBarrier(
        const nri::GlobalBarrierDesc& BarrierDesc)
    {
        m_GlobalBarriersCache.emplace_back(BarrierDesc);
    }

    //

    void ResourceStateTracker::BeginTracking(
        nri::Buffer*               Buffer,
        AtomBufferSubresourceState InitialState)
    {
        AME_LOG_ASSERT(Log::Rhi(), !m_CurrentStates.Buffers.contains(Buffer), "Buffer already being tracked");
        m_CurrentStates.Buffers[Buffer] = InitialState;
    }

    void ResourceStateTracker::BeginTracking(
        nri::CoreInterface&         NriCore,
        nri::Texture*               Texture,
        AtomTextureSubresourceState InitialState)
    {
        AME_LOG_ASSERT(Log::Rhi(), !m_CurrentStates.Textures.contains(Texture), "Texture already being tracked");

        auto& Desc = NriCore.GetTextureDesc(*Texture);

        for (auto [Mip, Array] : ForEachSubresource(0, Desc.mipNum, 0, Desc.arraySize))
        {
            SubresourceIndex Index(Mip, Array);
            m_CurrentStates.Textures[Texture][Index] = InitialState;
        }
    }

    void ResourceStateTracker::EndTracking(
        nri::Buffer* Buffer)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Buffers.contains(Buffer), "Buffer is not being tracked");
        m_CurrentStates.Buffers.erase(Buffer);
        m_PendingStates.Buffers.erase(Buffer);
    }

    void ResourceStateTracker::EndTracking(
        nri::Texture* Texture)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(Texture), "Texture is not being tracked");
        m_CurrentStates.Textures.erase(Texture);
        m_PendingStates.Textures.erase(Texture);
    }

    //

    void ResourceStateTracker::MutateState(
        nri::Buffer*               Buffer,
        AtomBufferSubresourceState State)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Buffers.contains(Buffer), "Buffer is not being tracked");
        m_CurrentStates.Buffers[Buffer] = State;
    }

    void ResourceStateTracker::MutateState(
        nri::CoreInterface&         NriCore,
        nri::Texture*               Texture,
        AtomTextureSubresourceState State,
        nri::Mip_t                  MipLevel,
        nri::Mip_t                  MipCount,
        nri::Dim_t                  ArraySlice,
        nri::Dim_t                  ArrayCount)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CurrentStates.Textures.contains(Texture), "Texture is not being tracked");

        auto& Desc          = NriCore.GetTextureDesc(*Texture);
        auto& CurrentStates = m_CurrentStates.Textures[Texture];

        if (MipCount == nri::REMAINING_MIP_LEVELS)
        {
            MipCount = Desc.mipNum - MipLevel;
        }
        if (ArrayCount == nri::REMAINING_ARRAY_LAYERS)
        {
            ArrayCount = Desc.arraySize - ArraySlice;
        }

        for (auto [Mip, Array] : ForEachSubresource(MipLevel, MipCount, ArraySlice, ArrayCount))
        {
            SubresourceIndex Index(Mip, Array);
            CurrentStates[Index] = State;
        }
    }

    //

    void ResourceStateTracker::CommitBarriers(
        nri::CoreInterface& NriCore,
        nri::CommandBuffer& CommandBuffer)
    {
        if (m_PendingStates.Buffers.empty() && m_PendingStates.Textures.empty())
        {
            return;
        }

        auto Buffers  = FlushBuffers();
        auto Textures = FlushTextures(NriCore);

        nri::BarrierGroupDesc Barriers{
            .globals    = m_GlobalBarriersCache.data(),
            .buffers    = Buffers.data(),
            .textures   = Textures.data(),
            .globalNum  = static_cast<uint16_t>(m_GlobalBarriersCache.size()),
            .bufferNum  = static_cast<uint16_t>(Buffers.size()),
            .textureNum = static_cast<uint16_t>(Textures.size())
        };

        NriCore.CmdBarrier(CommandBuffer, Barriers);

        m_PendingStates.Buffers.clear();
        m_PendingStates.Textures.clear();
        m_GlobalBarriersCache.clear();
    }

    //

    std::vector<nri::BufferBarrierDesc> ResourceStateTracker::FlushBuffers()
    {
        std::vector<nri::BufferBarrierDesc> Barriers;
        for (auto& [Buffer, States] : m_PendingStates.Buffers)
        {
            auto& CurrentState = m_CurrentStates.Buffers[Buffer];

            nri::BufferBarrierDesc ResourceBarrier{
                .buffer = Buffer,
                .before = CurrentState,
                .after  = CollapseStates(States)
            };

            CurrentState = ResourceBarrier.after;
            Barriers.push_back(std::move(ResourceBarrier));
        }
        return Barriers;
    }

    //

    std::vector<nri::TextureBarrierDesc> ResourceStateTracker::FlushTextures(
        nri::CoreInterface& NriCore)
    {
        std::vector<nri::TextureBarrierDesc> Barriers;
        for (auto& [Texture, SubresourceStates] : m_PendingStates.Textures)
        {
            auto TempBarriers = TransitionTexture(NriCore, Texture, SubresourceStates);
            Barriers.insert(Barriers.end(), std::make_move_iterator(TempBarriers.begin()), std::make_move_iterator(TempBarriers.end()));
        }
        return Barriers;
    }

    std::vector<nri::TextureBarrierDesc> ResourceStateTracker::TransitionTexture(
        nri::CoreInterface&                   NriCore,
        nri::Texture*                         Texture,
        const TextureSubresourceStates<true>& NewStates)
    {
        auto& CurrentSubresources = m_CurrentStates.Textures[Texture];

        bool StatesMatch = true;

        auto& FirstOldState = CurrentSubresources.begin()->second;
        auto  FirstNewState = CollapseStates(NewStates.begin()->second);

        std::vector<nri::TextureBarrierDesc> Barriers;

        auto ALSEquals = [](const nri::AccessLayoutStage& A, const nri::AccessLayoutStage& B)
        { return A.access == B.access && A.layout == B.layout && A.stages == B.stages; };

        for (auto& [Index, NewSubresources] : NewStates)
        {
            auto& CurrentState  = CurrentSubresources[Index];
            auto  FinalNewState = CollapseStates(NewSubresources);

            if (IsNewStateRedundant(CurrentState.access, FinalNewState.access))
            {
                continue;
            }

            auto OldState = std::exchange(CurrentState, FinalNewState);

            SubresourceIndex Subresource(Index);
            Barriers.emplace_back(nri::TextureBarrierDesc{
                .texture     = Texture,
                .before      = OldState,
                .after       = FinalNewState,
                .mipOffset   = Subresource.MipLevel,
                .mipNum      = 1,
                .arrayOffset = Subresource.ArraySlice,
                .arraySize   = 1 });

            // If any old subresource states do not match or any of the new states do not match
            // then performing single transition barrier for all subresources is not possible
            if (ALSEquals(OldState, FirstOldState) || !ALSEquals(FinalNewState, FirstNewState))
            {
                StatesMatch = false;
            }
        }

        // If multiple transitions were requested, but it's possible to make just one - do it
        if (StatesMatch && Barriers.size() > 1)
        {
            Barriers.resize(1);

            auto& Desc            = NriCore.GetTextureDesc(*Texture);
            Barriers[0].mipNum    = Desc.mipNum;
            Barriers[0].arraySize = Desc.arraySize;
        }

        return Barriers;
    }

    //

    auto ResourceStateTracker::CollapseStates(
        const AtomTextureSubresourceStateList<true>& States) -> AtomTextureSubresourceStateList<false>
    {
        AtomTextureSubresourceStateList<false> Combined{};
        if (!States.empty())
        {
            Combined.layout = States[0].layout;

            bool HasAllState = false;
            for (auto& State : States)
            {
                HasAllState |= State.stages == nri::StageBits::ALL;

                Combined.access |= State.access;
                Combined.stages |= State.stages;

                AME_LOG_ASSERT(Log::Rhi(), Combined.layout == State.layout, "Inconsistent layout");
            }

            if (HasAllState)
            {
                Combined.stages = nri::StageBits::ALL;
            }
        }
        return Combined;
    }

    auto ResourceStateTracker::CollapseStates(
        const AtomBufferSubresourceStateList<true>& States) -> AtomBufferSubresourceStateList<false>
    {
        AtomBufferSubresourceStateList<false> Combined{};

        bool HasAllState = false;
        for (auto& State : States)
        {
            HasAllState |= State.stages == nri::StageBits::ALL;
            Combined.access |= State.access;
            Combined.stages |= State.stages;
        }

        if (HasAllState)
        {
            Combined.stages = nri::StageBits::ALL;
        }
        return Combined;
    }

    bool ResourceStateTracker::IsNewStateRedundant(
        nri::AccessBits Current,
        nri::AccessBits Next)
    {
        constexpr nri::AccessBits ReadOnlyStates =
            nri::AccessBits::VERTEX_BUFFER |
            nri::AccessBits::CONSTANT_BUFFER |
            nri::AccessBits::INDEX_BUFFER |
            nri::AccessBits::DEPTH_STENCIL_READ |
            nri::AccessBits::SHADER_RESOURCE |
            nri::AccessBits::ACCELERATION_STRUCTURE_READ |
            nri::AccessBits::ARGUMENT_BUFFER |
            nri::AccessBits::COPY_SOURCE;

        return (Current == Next) ||
               ((Current & ReadOnlyStates) && (static_cast<uint32_t>(Current & Next) == static_cast<uint32_t>(Next)));
    }
} // namespace Ame::Rhi