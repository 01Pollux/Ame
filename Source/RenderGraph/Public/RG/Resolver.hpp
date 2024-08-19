#pragma once

#include <Rg/Resource.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Rg
{
    class Resolver
    {
        friend class PassStorage;

    public:
        Resolver(
            ResourceStorage& resourceStorage);

    public:
        /// <summary>
        /// Helper function to get device of the engine
        /// </summary>
        [[nodiscard]] Rhi::IRhiDevice* GetDevice() const noexcept;

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

        /// <summary>
        /// Create texture
        /// </summary>
        Dg::ITexture* CreateImmediateTexture(
            const ResourceId&      id,
            const Dg::TextureData* data,
            const Dg::TextureDesc& desc);

        /// <summary>
        /// Creates buffer immediately
        /// </summary>
        Dg::IBuffer* CreateImmediateBuffer(
            const ResourceId&     id,
            const Dg::BufferData* data,
            const Dg::BufferDesc& desc);

    public:
        [[nodiscard]] const Dg::BufferDesc* GetBufferDesc(
            const ResourceId& id) const;

        [[nodiscard]] const Dg::TextureDesc* GetTextureDesc(
            const ResourceId& id) const;

    public:
        /// <summary>
        /// import buffer to be used later when dispatching passes
        /// </summary>
        void ImportBuffer(
            const ResourceId& id,
            Ptr<Dg::IBuffer>  buffer);

        /// <summary>
        /// import texture to be used later when dispatching passes
        /// </summary>
        void ImportTexture(
            const ResourceId& id,
            Ptr<Dg::ITexture> texture);

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

    public:
        /// <summary>
        /// Write user data to resource storage
        /// </summary>
        void WriteUserData(
            const ResourceId& id,
            IObject*          userData);

        /// <summary>
        /// Read user data from resource storage
        /// </summary>
        void ReadUserData(
            const ResourceId& id);

        /// <summary>
        /// Get user data from id
        /// </summary>
        [[nodiscard]] IObject* GetUserData(
            const ResourceId& id) const;

        /// <summary>
        /// Get user data from id
        /// </summary>
        template<typename Ty>
        [[nodiscard]] Ptr<Ty> GetUserData(
            const ResourceId& id,
            const UId&        iid) const
        {
            return GetUserData<Ty>(id, iid);
        }

    private:
        Ref<ResourceStorage> m_Storage;

        std::vector<ResourceViewId> m_RenderTargets;
        ResourceViewId              m_DepthStencil;

        std::set<ResourceId> m_ResourcesCreated;
        std::set<ResourceId> m_ResourcesRead;
        std::set<ResourceId> m_ResourcesWritten;
    };
} // namespace Ame::Rg