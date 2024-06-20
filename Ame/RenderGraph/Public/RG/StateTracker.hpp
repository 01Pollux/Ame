#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <type_traits>

#include <Core/Coroutine.hpp>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>
#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Descs/View.hpp>

namespace Ame::RG
{
    class ResourceStateTracker
    {
        using SubresourceIndexType = uint32_t;

        struct SubresourceIndex
        {
            Rhi::Mip_t MipLevel;
            Rhi::Dim_t ArrayIndex;

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
                Rhi::Mip_t mipLevel,
                Rhi::Dim_t arraySlice) :
                MipLevel(mipLevel),
                ArrayIndex(arraySlice)
            {
            }
        };

        using AtomicResourceState = Rhi::AccessStage;

        using AtomicTextureSubresourceState = Rhi::AccessLayoutStage;
        using AtomicBufferSubresourceState  = Rhi::AccessStage;

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
        using GlobalBarrierList        = std::vector<Rhi::GlobalBarrierDesc>;

    public:
        ResourceStateTracker(
            const Rhi::DeviceDesc& deviceDesc);

        ResourceStateTracker(const ResourceStateTracker&) = delete;
        ResourceStateTracker(ResourceStateTracker&&)      = delete;

        ResourceStateTracker& operator=(const ResourceStateTracker&) = delete;
        ResourceStateTracker& operator=(ResourceStateTracker&&)      = delete;

        ~ResourceStateTracker() = default;

    public:
        /// <summary>
        /// Initialize the state tracker
        /// </summary>
        void Initialize(
            const Rhi::DeviceDesc& deviceDesc);

    public:
        /// <summary>
        /// Query the current state of a buffer
        /// </summary>
        [[nodiscard]] AtomicBufferSubresourceState QueryState(
            const Rhi::Buffer& buffer) const;

        /// <summary>
        /// Require a texture to be in a certain state
        /// if Append is true, the state will be appended to the next state
        /// </summary>
        [[nodiscard]] Co::generator<AtomicTextureSubresourceState> QueryState(
            const Rhi::Texture&            texture,
            const Rhi::TextureSubresource& subresource = Rhi::c_AllSubresources) const;

    public:
        /// <summary>
        /// Require a buffer to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            const Rhi::Buffer&           buffer,
            AtomicBufferSubresourceState state,
            bool                         append = false);

        /// <summary>
        /// Require a texture to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            const Rhi::Texture&            texture,
            AtomicTextureSubresourceState  state,
            const Rhi::TextureSubresource& subresource = Rhi::c_AllSubresources,
            bool                           append      = false);

        /// <summary>
        /// Require a texture to be in a certain states
        /// size of states must be same as number of subresources for texture
        /// if Append is true, the state will be appended to the next state
        /// </summary>
        void RequireStates(
            const Rhi::Texture&                            texture,
            std::span<const AtomicTextureSubresourceState> states,
            bool                                           append = false);

        /// <summary>
        /// Place a global barrier
        /// </summary>
        void PlaceBarrier(
            const Rhi::GlobalBarrierDesc& barrierDesc);

    public:
        /// <summary>
        /// Begin tracking a buffer
        /// </summary>
        void BeginTracking(
            const Rhi::Buffer&           buffer,
            AtomicBufferSubresourceState initialState = { Rhi::AccessBits::UNKNOWN, Rhi::StageBits::ALL });

        /// <summary>
        /// Begin tracking a texture
        /// </summary>
        void BeginTracking(
            const Rhi::Texture&           texture,
            AtomicTextureSubresourceState initialState = { Rhi::AccessBits::UNKNOWN, Rhi::LayoutType::UNKNOWN, Rhi::StageBits::ALL });

        /// <summary>
        /// End tracking a buffer
        /// </summary>
        void EndTracking(
            const Rhi::Buffer& buffer);

        /// <summary>
        /// End tracking a texture
        /// </summary>
        void EndTracking(
            const Rhi::Texture& texture);

    public:
        /// <summary>
        /// Commit all the pending barriers
        /// </summary>
        void CommitBarriers(
            Rhi::CommandList& commandList);

    private:
        /// <summary>
        /// Flush all the buffers to the current state
        /// </summary>
        [[nodiscard]] std::vector<Rhi::BufferBarrierDesc> FlushBuffers();

        /// <summary>
        /// Flush all the texture to the current state
        /// </summary>
        [[nodiscard]] std::vector<Rhi::TextureBarrierDesc> FlushTextures();

        /// <summary>
        /// Flush all texture to the current state
        /// </summary>
        [[nodiscard]] std::vector<Rhi::TextureBarrierDesc> TransitionTexture(
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
            Rhi::AccessBits current,
            Rhi::AccessBits next);

    private:
        /// <summary>
        /// Compare two access stages
        /// </summary>
        [[nodiscard]] bool AreStateEqual(
            Rhi::AccessStage a,
            Rhi::AccessStage b) const;

        /// <summary>
        /// Compare two states
        /// </summary>
        [[nodiscard]] bool AreStateEqual(
            const Rhi::AccessLayoutStage& a,
            const Rhi::AccessLayoutStage& b) const;

        /// <summary>
        /// Strip unsupported stages
        /// </summary>
        void StripUnsupportedStages(
            Rhi::StageBits& stages);

    private:
        CRef<Rhi::DeviceDesc> m_DeviceDesc;

        CurrentResourceStateMaps m_CurrentStates;
        PendingResourceStateMaps m_PendingStates;
        GlobalBarrierList        m_GlobalBarriersCache;
    };
} // namespace Ame::RG