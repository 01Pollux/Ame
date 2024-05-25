#pragma once

#include <map>
#include <Gfx/RG/Resource.hpp>
#include <Gfx/RG/Resources/FrameResource.hpp>
#include <Gfx/RG/EntityStore.hpp>

namespace Ame::Ecs
{
    class Universe;
} // namespace Ame::Ecs

namespace Ame::Gfx::RG
{
    class ResourceStorage
    {
        friend class Resolver;
        friend class Context;
        friend class DependencyLevel;

        using ResourceMapType = std::map<ResourceId, ResourceHandle>;

    public:
        ResourceStorage(
            Rhi::Device&   rhiDevice,
            Ecs::Universe& universe);

        ResourceStorage(const ResourceStorage&)     = delete;
        ResourceStorage(ResourceStorage&&) noexcept = default;

        ResourceStorage& operator=(const ResourceStorage&)     = delete;
        ResourceStorage& operator=(ResourceStorage&&) noexcept = default;

        ~ResourceStorage();

    public:
        /// <summary>
        /// Helper function to get device of the engine
        /// </summary>
        [[nodiscard]] Rhi::Device& GetDevice() const;

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
        [[nodiscard]] EntityStore GetEntityStore() const;

    public:
        /// <summary>
        /// Get frame for the current frame
        /// </summary>
        [[nodiscard]] const ResourceHandle& GetFrameResource() const;

        /// <summary>
        /// Get frame resource view for the current frame
        /// </summary>
        [[nodiscard]] const Rhi::ResourceView& GetFrameResourceHandle() const;

        /// <summary>
        /// Get frame resource data for the current frame
        /// </summary>
        [[nodiscard]] const FrameResourceCPU& GetFrameResourceData() const;

    public:
        /// <summary>
        /// Get resource from id
        /// </summary>
        [[nodiscard]] const ResourceHandle& GetResource(
            const ResourceId& id) const;

        /// <summary>
        /// Get resource from id
        /// </summary>
        [[nodiscard]] ResourceHandle& GetResourceMut(
            const ResourceId& id);

        /// <summary>
        /// Get resource from id
        /// </summary>
        [[nodiscard]] ResourceViewDesc& GetResourceViewDescMut(
            const ResourceViewId& viewId);

        /// <summary>
        /// Get resource from id
        /// </summary>
        [[nodiscard]] const ResourceViewDesc& GetResourceViewDesc(
            const ResourceViewId& viewId) const;

        /// <summary>
        /// Get resource view descriptor from id
        /// </summary>
        [[nodiscard]] const Rhi::ResourceView& GetResourceViewHandle(
            const ResourceViewId& viewId) const;

    private:
        /// <summary>
        /// Declare buffer to be created later when dispatching passes
        /// </summary>
        void DeclareBuffer(
            const ResourceId&      id,
            const Rhi::BufferDesc& desc);

        /// <summary>
        /// Declare texture to be created later when dispatching passes
        /// </summary>
        void DeclareTexture(
            const ResourceId&       id,
            const Rhi::TextureDesc& desc);

    public:
        /// <summary>
        /// import buffer to be used later when dispatching passes
        /// </summary>
        void ImportBuffer(
            const ResourceId& id,
            Rhi::Buffer       buffer);

        /// <summary>
        /// import texture to be used later when dispatching passes
        /// </summary>
        void ImportTexture(
            const ResourceId& id,
            Rhi::Texture      texture);

    private:
        /// <summary>
        /// Update and recreate resources if needed
        /// </summary>
        void UpdateResources();

        /// <summary>
        /// Declare resource view to be bound later when dispatching passes
        /// </summary>
        Rhi::BufferDesc& DeclareBufferView(
            const ResourceViewId& viewId,
            ResourceViewDesc      desc);

        /// <summary>
        /// Declare resource view to be bound later when dispatching passes
        /// </summary>
        Rhi::TextureDesc& DeclareTextureView(
            const ResourceViewId& viewId,
            ResourceViewDesc      desc);

    private:
        /// <summary>
        /// Reset camera storage resources
        /// </summary>
        void ResetCameraStorage();

        /// <summary>
        /// Update core resources such as frame resource, transform buffer, etc.
        /// </summary>
        void UpdateCoreResources();

    private:
        /// <summary>
        /// Update frame resource for the current frame
        /// </summary>
        void UpdateFrameResource(
            float                        engineTime,
            float                        gameTime,
            float                        deltaTime,
            const Ecs::Entity&           cameraEntity,
            const Math::TransformMatrix& transform,
            const Math::Matrix4x4&       projection,
            const Math::Vector2&         viewport);

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
        Ref<Rhi::Device> m_Device;

        UPtr<CoreResources>  m_CoreResources;
        ResourceMapType      m_Resources;
        std::set<ResourceId> m_ImportedResources;

#ifndef AME_DIST
        bool m_Locked = false;
#endif
    };
} // namespace Ame::Gfx::RG