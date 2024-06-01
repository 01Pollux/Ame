#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <type_traits>

#include <Core/Coroutine.hpp>
#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    class ResourceStateTracker
    {
        using SubresourceIndexType = uint32_t;

        struct SubresourceIndex
        {
            nri::Mip_t MipLevel;
            nri::Dim_t ArrayIndex;

            operator SubresourceIndexType() const
            {
                return (ArrayIndex << 16) | MipLevel;
            }

            constexpr explicit SubresourceIndex(
                uint32_t index) :
                MipLevel(index & 0xFFFF),
                ArrayIndex(index >> 16)
            {
            }

            constexpr SubresourceIndex(
                nri::Mip_t mipLevel,
                nri::Dim_t arraySlice) :
                MipLevel(mipLevel),
                ArrayIndex(arraySlice)
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
        using TextureSubresourceStates = std::map<SubresourceIndexType, AtomTextureSubresourceStateList<Many>>;
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
        /// Query the current state of a nriBuffer
        /// </summary>
        [[nodiscard]] AtomicBufferSubresourceState QueryState(
            nri::Buffer* nriBuffer) const;

        /// <summary>
        /// Require a nriTexture to be in a certain state
        /// if Append is true, the state will be appended to the next state
        /// </summary>
        [[nodiscard]] Co::generator<AtomicTextureSubresourceState> QueryState(
            nri::Texture* nriTexture,
            nri::Mip_t    mipLevel,
            nri::Mip_t    mipCount,
            nri::Dim_t    arraySlice,
            nri::Dim_t    arrayCount) const;

    public:
        /// <summary>
        /// Require a nriBuffer to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            nri::Buffer*                 nriBuffer,
            AtomicBufferSubresourceState state,
            bool                         append);

        /// <summary>
        /// Require a nriTexture to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            nri::CoreInterface&           nriCore,
            nri::Texture*                 nriTexture,
            AtomicTextureSubresourceState state,
            nri::Mip_t                    mipLevel,
            nri::Mip_t                    mipCount,
            nri::Dim_t                    arraySlice,
            nri::Dim_t                    arrayCount,
            bool                          append);

        /// <summary>
        /// Require a nriTexture to be in a certain states
        /// size of states must be same as number of subresources for nriTexture
        /// if Append is true, the state will be appended to the next state
        /// </summary>
        void RequireStates(
            nri::CoreInterface&                            nriCore,
            nri::Texture*                                  nriTexture,
            std::span<const AtomicTextureSubresourceState> states,
            bool                                           append);

        /// <summary>
        /// Place a global barrier
        /// </summary>
        void PlaceBarrier(
            const nri::GlobalBarrierDesc& barrierDesc);

    public:
        /// <summary>
        /// Begin tracking a nriBuffer
        /// </summary>
        void BeginTracking(
            nri::Buffer*                 nriBuffer,
            AtomicBufferSubresourceState initialState);

        /// <summary>
        /// Begin tracking a nriTexture
        /// </summary>
        void BeginTracking(
            nri::CoreInterface&           nriCore,
            nri::Texture*                 nriTexture,
            AtomicTextureSubresourceState initialState);

        /// <summary>
        /// End tracking a nriBuffer
        /// </summary>
        void EndTracking(
            nri::Buffer* nriBuffer);

        /// <summary>
        /// End tracking a nriTexture
        /// </summary>
        void EndTracking(
            nri::Texture* nriTexture);

    public:
        /// <summary>
        /// Mutate the state of a nriBuffer
        /// </summary>
        void MutateState(
            nri::Buffer*                 nriBuffer,
            AtomicBufferSubresourceState state);

        /// <summary>
        /// Mutate the state of a nriTexture
        /// </summary>
        void MutateState(
            nri::CoreInterface&           nriCore,
            nri::Texture*                 nriTexture,
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
        /// Flush all the nriTexture to the current state
        /// </summary>
        [[nodiscard]] std::vector<nri::TextureBarrierDesc> FlushTextures(
            nri::CoreInterface& nriCore);

        /// <summary>
        /// Flush all nriTexture to the current state
        /// </summary>
        [[nodiscard]] std::vector<nri::TextureBarrierDesc> TransitionTexture(
            nri::CoreInterface&                   nriCore,
            nri::Texture*                         nriTexture,
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