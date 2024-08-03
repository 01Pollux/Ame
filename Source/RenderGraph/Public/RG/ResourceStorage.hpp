#pragma once

#include <RG/Resource.hpp>

namespace Ame::RG
{
    class ResourceStorage
    {
        friend class Resolver;
        friend class Context;
        friend class Graph;
        friend class DependencyLevel;

        using ResourceMapType = std::map<ResourceId, ResourceHandle>;

    public:
        /// <summary>
        /// Check to see if resource of id exists
        /// </summary>
        [[nodiscard]] bool ContainsResource(
            const ResourceId& id) const;

        /// <summary>
        /// Check to see if resource of id exists
        /// </summary>
        [[nodiscard]] bool ContainsResourceView(
            const ResourceViewId& viewId);

    public:
        /// <summary>
        /// Get resource from id
        /// </summary>
        [[nodiscard]] const ResourceHandle* GetResource(
            const ResourceId& id) const;

        /// <summary>
        /// Get resource from id
        /// </summary>
        [[nodiscard]] ResourceHandle* GetResourceMut(
            const ResourceId& id);

        /// <summary>
        /// Get resource view descriptor from id
        /// </summary>
        [[nodiscard]] RhiResourceView GetResourceView(
            const ResourceViewId& viewId) const;

        /// <summary>
        /// Get resource view descriptor from id
        /// </summary>
        [[nodiscard]] Ptr<Dg::IObject> GetResourceViewHandle(
            const ResourceViewId& viewId) const;

        /// <summary>
        /// Get resource view descriptor from id as buffer view
        /// </summary>
        [[nodiscard]] Ptr<Dg::IBufferView> GetBufferViewHandleT(
            const ResourceViewId& viewId) const
        {
            auto view = GetResourceViewHandle(viewId);
            return view.Cast<Dg::IBufferView>(Dg::IID_BufferView);
        }

        /// <summary>
        /// Get resource view descriptor from id as texture view
        /// </summary>
        [[nodiscard]] Ptr<Dg::ITextureView> GetTextureViewHandleT(
            const ResourceViewId& viewId) const
        {
            auto view = GetResourceViewHandle(viewId);
            return view.Cast<Dg::ITextureView>(Dg::IID_TextureView);
        }

    private:
        /// <summary>
        /// Declare buffer to be created later when dispatching passes
        /// </summary>
        void DeclareResource(
            const ResourceId& id,
            RhiResource       resource);

    public:
        /// <summary>
        /// import buffer to be used later when dispatching passes
        /// </summary>
        void ImportBuffer(
            const ResourceId& id,
            Ptr<Dg::IBuffer>  buffer);

        /// <summary>
        /// import texture to be used later when dispatching passes
        /// Importing backbuffer texture might result in device removal if not handled properly (discard resource when window is resized)
        /// </summary>
        void ImportTexture(
            const ResourceId& id,
            Ptr<Dg::ITexture> texture);

        /// <summary>
        /// Remove imported resource
        /// asserts if resource is not imported
        /// </summary>
        void DiscardResource(
            const ResourceId& id);

    private:
        /// <summary>
        /// Update and recreate resources if needed
        /// </summary>
        void UpdateResources(
            Dg::IRenderDevice* renderDevice);

        /// <summary>
        /// Declare resource view to be bound later when dispatching passes
        /// </summary>
        BufferResource& DeclareBufferView(
            const ResourceViewId&         viewId,
            const BufferResourceViewDesc& desc);

        /// <summary>
        /// Declare resource view to be bound later when dispatching passes
        /// </summary>
        TextureResource& DeclareTextureView(
            const ResourceViewId&          viewId,
            const TextureResourceViewDesc& desc);

    private:
        /// <summary>
        /// Lock storage to prevent further changes
        /// DEBUG ONLY
        /// </summary>
        void Lock();

        /// <summary>
        /// Unlock storage to allow further changes
        /// DEBUG ONLY
        /// </summary>
        void Unlock();

        /// <summary>
        /// Check if storage is locked or not
        /// DEBUG ONLY
        /// </summary>
        void CheckLockState(
            bool locked) const;

    private:
        /// <summary>
        /// Check if imported resources were changed and needs to be rebuilt
        /// </summary>
        [[nodiscard]] bool NeedsRebuild() const noexcept;

        /// <summary>
        /// Set rebuild state
        /// </summary>
        void SetRebuildState(
            bool state) noexcept;

    private:
        ResourceMapType m_Resources;

        bool m_NeedRebuild : 1 = false;
#ifndef AME_DIST
        bool m_Locked : 1 = false;
#endif
    };
} // namespace Ame::RG