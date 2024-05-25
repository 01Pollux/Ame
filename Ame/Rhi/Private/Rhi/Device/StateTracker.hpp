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
                uint32_t index) :
                MipLevel(index >> 16),
                ArraySlice(index & 0xFFFF)
            {
            }

            constexpr SubresourceIndex(
                nri::Mip_t mipLevel,
                nri::Dim_t arraySlice) :
                MipLevel(mipLevel),
                ArraySlice(arraySlice)
            {
            }
        };

        using AtomicResourceState = nri::AccessStage;

        using AtomicTextureSubresourceState = nri::AccessLayoutStage;
        using AtomicBufferSubresourceState  = nri::AccessStage;

        template<bool Many>
        using AtomTextureSubresourceStateList = std::conditional_t<Many, std::vector<AtomicTextureSubresourceState>, AtomicTextureSubresourceState>;
        template<bool Many>
        using AtomBufferSubresourceStateList = std::conditional_t<Many, std::vector<AtomicBufferSubresourceState>, AtomicBufferSubresourceState>;

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
            const nri::DeviceDesc* deviceDesc);

    public:
        /// <summary>
        /// Require a buffer to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            nri::Buffer*                 buffer,
            AtomicBufferSubresourceState state,
            bool                         append = false);

        /// <summary>
        /// Require a texture to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            nri::CoreInterface&           nriCore,
            nri::Texture*                 texture,
            AtomicTextureSubresourceState state,
            nri::Mip_t                    mipLevel   = 0,
            nri::Mip_t                    mipCount   = nri::REMAINING_MIP_LEVELS,
            nri::Dim_t                    arraySlice = 0,
            nri::Dim_t                    arrayCount = nri::REMAINING_ARRAY_LAYERS,
            bool                          append     = false);

        /// <summary>
        /// Place a global barrier
        /// </summary>
        void PlaceBarrier(
            const nri::GlobalBarrierDesc& barrierDesc);

    public:
        /// <summary>
        /// Begin tracking a buffer
        /// </summary>
        void BeginTracking(
            nri::Buffer*                 buffer,
            AtomicBufferSubresourceState initialState);

        /// <summary>
        /// Begin tracking a texture
        /// </summary>
        void BeginTracking(
            nri::CoreInterface&           nriCore,
            nri::Texture*                 texture,
            AtomicTextureSubresourceState initialState);

        /// <summary>
        /// End tracking a buffer
        /// </summary>
        void EndTracking(
            nri::Buffer* buffer);

        /// <summary>
        /// End tracking a texture
        /// </summary>
        void EndTracking(
            nri::Texture* texture);

    public:
        /// <summary>
        /// Mutate the state of a buffer
        /// </summary>
        void MutateState(
            nri::Buffer*                 buffer,
            AtomicBufferSubresourceState state);

        /// <summary>
        /// Mutate the state of a texture
        /// </summary>
        void MutateState(
            nri::CoreInterface&           nriCore,
            nri::Texture*                 texture,
            AtomicTextureSubresourceState state,
            nri::Mip_t                    mipLevel   = 0,
            nri::Mip_t                    mipCount   = nri::REMAINING_MIP_LEVELS,
            nri::Dim_t                    arraySlice = 0,
            nri::Dim_t                    arrayCount = nri::REMAINING_ARRAY_LAYERS);

    public:
        /// <summary>
        /// Commit all the pending barriers
        /// </summary>
        void CommitBarriers(
            nri::CoreInterface& nriCore,
            nri::CommandBuffer& commandBuffer);

    private:
        /// <summary>
        /// Flush all the buffers to the current state
        /// </summary>
        [[nodiscard]] std::vector<nri::BufferBarrierDesc> FlushBuffers();

        /// <summary>
        /// Flush all the texture to the current state
        /// </summary>
        [[nodiscard]] std::vector<nri::TextureBarrierDesc> FlushTextures(
            nri::CoreInterface& nriCore);

        /// <summary>
        /// Flush all texture to the current state
        /// </summary>
        [[nodiscard]] std::vector<nri::TextureBarrierDesc> TransitionTexture(
            nri::CoreInterface&                   nriCore,
            nri::Texture*                         texture,
            const TextureSubresourceStates<true>& newStates);

    private:
        /// <summary>
        /// Combine states into one
        /// </summary>
        [[nodiscard]] static AtomTextureSubresourceStateList<false> CollapseStates(
            const AtomTextureSubresourceStateList<true>& states);

        /// <summary>
        /// Combine states into one
        /// </summary>
        [[nodiscard]] static AtomBufferSubresourceStateList<false> CollapseStates(
            const AtomBufferSubresourceStateList<true>& states);

        /// <summary>
        /// Transition is redundant if either states completely match
        /// or current state is a read state and new state is a partial or complete subset of the current
        /// (which implies that it is also a read state)
        /// </summary>
        [[nodiscard]] static bool IsNewStateRedundant(
            nri::AccessBits current,
            nri::AccessBits next);

    private:
        /// <summary>
        /// Compare two access stages
        /// </summary>
        [[nodiscard]] bool AreStateEqual(
            nri::AccessStage a,
            nri::AccessStage b) const;

        /// <summary>
        /// Compare two states
        /// </summary>
        [[nodiscard]] bool AreStateEqual(
            const nri::AccessLayoutStage& a,
            const nri::AccessLayoutStage& b) const;

        /// <summary>
        /// Strip unsupported stages
        /// </summary>
        void StripUnsupportedStages(
            nri::StageBits& stages);

    private:
        const nri::DeviceDesc*   m_DeviceDesc = nullptr;
        CurrentResourceStateMaps m_CurrentStates;
        PendingResourceStateMaps m_PendingStates;
        GlobalBarrierList        m_GlobalBarriersCache;
    };
} // namespace Ame::Rhi