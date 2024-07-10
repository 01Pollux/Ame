#pragma once

#include <RG/Resource.hpp>

namespace Ame::RG
{
    class Resolver
    {
        friend class PassStorage;

    public:
        Resolver(
            Rhi::RhiDevice&  rhiDevice,
            ResourceStorage& resourceStorage);

    public:
        /// <summary>
        /// Helper function to get device of the engine
        /// </summary>
        [[nodiscard]] Rhi::RhiDevice& GetDevice() const;

        /// <summary>
        /// Get backbuffer texture format
        /// </summary>
        [[nodiscard]] Dg::TEXTURE_FORMAT GetBackbufferFormat() const;

        /// <summary>
        /// Get backbuffer texture desc
        /// </summary>
        [[nodiscard]] const Dg::TextureDesc& GetBackbufferDesc() const;

    public:
        /// <summary>
        /// Create buffer
        /// </summary>
        void CreateBuffer(
            const ResourceId&     id,
            const Dg::BufferDesc& desc);

        /// <summary>
        /// Create texture
        /// </summary>
        void CreateTexture(
            const ResourceId&      id,
            const Dg::TextureDesc& desc);

    public:
        /// <summary>
        /// import buffer to be used later when dispatching passes
        /// </summary>
        void ImportBuffer(
            const ResourceId&    id,
            Dg::Ptr<Dg::IBuffer> buffer);

        /// <summary>
        /// import texture to be used later when dispatching passes
        /// </summary>
        void ImportTexture(
            const ResourceId&     id,
            Dg::Ptr<Dg::ITexture> texture);

    public:
        /// <summary>
        /// Write to buffer resource
        /// </summary>
        void WriteBuffer(
            const ResourceViewId&         viewId,
            Dg::BIND_FLAGS                bindFlags,
            const BufferResourceViewDesc& viewDesc);

        /// <summary>
        /// Write to resource
        /// </summary>
        void WriteTexture(
            const ResourceViewId&          viewId,
            Dg::BIND_FLAGS                 bindFlags,
            const TextureResourceViewDesc& viewDesc);

        /// <summary>
        /// Write resource view to add dependency without actually writing anything
        /// </summary>
        void WriteResourceEmpty(
            const ResourceId& Id);

    public:
        /// <summary>
        /// Read from buffer resource
        /// </summary>
        void ReadBuffer(
            const ResourceViewId&         viewId,
            Dg::BIND_FLAGS                bindFlags,
            const BufferResourceViewDesc& viewDesc);

        /// <summary>
        /// Read from texture resource
        /// </summary>
        void ReadTexture(
            const ResourceViewId&          viewId,
            Dg::BIND_FLAGS                 bindFlags,
            const TextureResourceViewDesc& viewDesc);

        /// <summary>
        /// Read dummy resource
        /// </summary>
        void ReadResourceEmpty(
            const ResourceId& id);

    private:
        Ref<Rhi::RhiDevice>  m_RhiDevice;
        Ref<ResourceStorage> m_Storage;

        std::vector<ResourceViewId> m_RenderTargets;
        ResourceViewId              m_DepthStencil;

        std::set<ResourceId> m_ResourcesCreated;
        std::set<ResourceId> m_ResourcesRead;
        std::set<ResourceId> m_ResourcesWritten;
    };
} // namespace Ame::RG