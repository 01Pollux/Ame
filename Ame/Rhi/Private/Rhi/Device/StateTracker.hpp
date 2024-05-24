#pragma once

#include <Core/Ame.hpp>
#include <vector>
#include <unordered_map>
#include <type_traits>

#include "../Nri/Nri.hpp"

namespace Ame::Rhi
{
    class ResourceStateTracker
    {
        using SubresourceIndexType = uint32_t;

        struct SubresourceIndex
        {
            nri::Mip_t MipLevel;
            nri::Dim_t ArraySlice;

            operator SubresourceIndexType() const
            {
                return (MipLevel << 16) | ArraySlice;
            }

            constexpr explicit SubresourceIndex(
                uint32_t Index) :
                MipLevel(Index >> 16),
                ArraySlice(Index & 0xFFFF)
            {
            }

            constexpr SubresourceIndex(
                nri::Mip_t MipLevel,
                nri::Dim_t ArraySlice) :
                MipLevel(MipLevel),
                ArraySlice(ArraySlice)
            {
            }
        };

        using AtomResourceState = nri::AccessStage;

        using AtomTextureSubresourceState = nri::AccessLayoutStage;
        using AtomBufferSubresourceState  = nri::AccessStage;

        template<bool Many>
        using AtomTextureSubresourceStateList = std::conditional_t<Many, std::vector<AtomTextureSubresourceState>, AtomTextureSubresourceState>;
        template<bool Many>
        using AtomBufferSubresourceStateList = std::conditional_t<Many, std::vector<AtomBufferSubresourceState>, AtomBufferSubresourceState>;

        template<bool Many>
        using TextureSubresourceStates = std::unordered_map<SubresourceIndexType, AtomTextureSubresourceStateList<Many>>;
        template<bool Many>
        using BufferSubresourceStates = AtomBufferSubresourceStateList<Many>;

        template<bool Many>
        using TextureStateMap = std::unordered_map<nri::Texture*, TextureSubresourceStates<Many>>;
        template<bool Many>
        using BufferStateMap = std::unordered_map<nri::Buffer*, BufferSubresourceStates<Many>>;

        template<bool IsPending>
        struct ResourceStateMaps
        {
            TextureStateMap<IsPending> Textures;
            BufferStateMap<IsPending>  Buffers;
        };

        using PendingResourceStateMaps = ResourceStateMaps<true>;
        using CurrentResourceStateMaps = ResourceStateMaps<false>;
        using GlobalBarrierList        = std::vector<nri::GlobalBarrierDesc>;

    public:
        /// <summary>
        /// Initialize the state tracker
        /// </summary>
        void Initialize(
            const nri::DeviceDesc* DeviceDesc);

    public:
        /// <summary>
        /// Require a buffer to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            nri::Buffer*               Buffer,
            AtomBufferSubresourceState State,
            bool                       Append = false);

        /// <summary>
        /// Require a texture to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            nri::CoreInterface&         NriCore,
            nri::Texture*               Texture,
            AtomTextureSubresourceState State,
            nri::Mip_t                  MipLevel   = 0,
            nri::Mip_t                  MipCount   = nri::REMAINING_MIP_LEVELS,
            nri::Dim_t                  ArraySlice = 0,
            nri::Dim_t                  ArrayCount = nri::REMAINING_ARRAY_LAYERS,
            bool                        Append     = false);

        /// <summary>
        /// Place a global barrier
        /// </summary>
        void PlaceBarrier(
            const nri::GlobalBarrierDesc& BarrierDesc);

    public:
        /// <summary>
        /// Begin tracking a buffer
        /// </summary>
        void BeginTracking(
            nri::Buffer*               Buffer,
            AtomBufferSubresourceState InitialState);

        /// <summary>
        /// Begin tracking a texture
        /// </summary>
        void BeginTracking(
            nri::CoreInterface&         NriCore,
            nri::Texture*               Texture,
            AtomTextureSubresourceState InitialState);

        /// <summary>
        /// End tracking a buffer
        /// </summary>
        void EndTracking(
            nri::Buffer* Buffer);

        /// <summary>
        /// End tracking a texture
        /// </summary>
        void EndTracking(
            nri::Texture* Texture);

    public:
        /// <summary>
        /// Mutate the state of a buffer
        /// </summary>
        void MutateState(
            nri::Buffer*               Buffer,
            AtomBufferSubresourceState State);

        /// <summary>
        /// Mutate the state of a texture
        /// </summary>
        void MutateState(
            nri::CoreInterface&         NriCore,
            nri::Texture*               Texture,
            AtomTextureSubresourceState State,
            nri::Mip_t                  MipLevel   = 0,
            nri::Mip_t                  MipCount   = nri::REMAINING_MIP_LEVELS,
            nri::Dim_t                  ArraySlice = 0,
            nri::Dim_t                  ArrayCount = nri::REMAINING_ARRAY_LAYERS);

    public:
        /// <summary>
        /// Commit all the pending barriers
        /// </summary>
        void CommitBarriers(
            nri::CoreInterface& NriCore,
            nri::CommandBuffer& CommandBuffer);

    private:
        /// <summary>
        /// Flush all the buffers to the current state
        /// </summary>
        [[nodiscard]] std::vector<nri::BufferBarrierDesc> FlushBuffers();

        /// <summary>
        /// Flush all the texture to the current state
        /// </summary>
        [[nodiscard]] std::vector<nri::TextureBarrierDesc> FlushTextures(
            nri::CoreInterface& NriCore);

        /// <summary>
        /// Flush all texture to the current state
        /// </summary>
        [[nodiscard]] std::vector<nri::TextureBarrierDesc> TransitionTexture(
            nri::CoreInterface&                   NriCore,
            nri::Texture*                         Texture,
            const TextureSubresourceStates<true>& NewStates);

    private:
        /// <summary>
        /// Combine states into one
        /// </summary>
        [[nodiscard]] static AtomTextureSubresourceStateList<false> CollapseStates(
            const AtomTextureSubresourceStateList<true>& States);

        /// <summary>
        /// Combine states into one
        /// </summary>
        [[nodiscard]] static AtomBufferSubresourceStateList<false> CollapseStates(
            const AtomBufferSubresourceStateList<true>& States);

        /// <summary>
        /// Transition is redundant if either states completely match
        /// or current state is a read state and new state is a partial or complete subset of the current
        /// (which implies that it is also a read state)
        /// </summary>
        [[nodiscard]] static bool IsNewStateRedundant(
            nri::AccessBits Current,
            nri::AccessBits Next);

    private:
        /// <summary>
        /// Compare two access stages
        /// </summary>
        [[nodiscard]] bool AreStateEqual(
            nri::AccessStage A,
            nri::AccessStage B) const;

        /// <summary>
        /// Compare two states
        /// </summary>
        [[nodiscard]] bool AreStateEqual(
            const nri::AccessLayoutStage& A,
            const nri::AccessLayoutStage& B) const;

        /// <summary>
        /// Strip unsupported stages
        /// </summary>
        void StripUnsupportedStages(
            nri::StageBits& Stages);

    private:
        const nri::DeviceDesc*   m_DeviceDesc = nullptr;
        CurrentResourceStateMaps m_CurrentStates;
        PendingResourceStateMaps m_PendingStates;
        GlobalBarrierList        m_GlobalBarriersCache;
    };
} // namespace Ame::Rhi