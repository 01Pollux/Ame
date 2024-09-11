#pragma once

#include <Rg/Resource.hpp>

namespace Ame::Rg
{
    class ResourceStorage
    {
        friend class Resolver;
        friend class Context;
        friend class Graph;
        friend class DependencyLevel;

        using ResourceMapType = std::map<ResourceId, ResourceHandle>;
        using UserDataMapType = std::map<ResourceId, Ptr<IObject>>;

    public:
        ResourceStorage(Rhi::IRhiDevice* rhiDevice) : m_RhiDevice(rhiDevice)
        {
        }

    public:
        [[nodiscard]] Rhi::IRhiDevice*       GetDevice() const noexcept;
        [[nodiscard]] Dg::TEXTURE_FORMAT     GetBackbufferFormat() const;
        [[nodiscard]] const Dg::TextureDesc& GetBackbufferDesc() const;

    public:
        [[nodiscard]] bool                  ContainsResource(const ResourceId& id) const;
        [[nodiscard]] const ResourceHandle* GetResource(const ResourceId& id) const;
        [[nodiscard]] ResourceHandle*       GetResourceMut(const ResourceId& id);

    public:
        [[nodiscard]] Dg::IBufferView*  GetBufferView(ResourceViewId viewId) const;
        [[nodiscard]] Dg::ITextureView* GetTextureView(ResourceViewId viewId) const;

    public:
        [[nodiscard]] bool                          ContainsUserData(const ResourceId& id) const;
        [[nodiscard]] void                          SetUserData(const ResourceId& id, IObject* object);
        [[nodiscard]] IObject*                      GetUserData(const ResourceId& id) const;
        template<typename Ty> [[nodiscard]] Ptr<Ty> GetUserData(const ResourceId& id, const UId& iid) const
        {
            return { GetUserData(id), iid };
        }

    private:
        void DeclareResource(const ResourceId& id, Rhi::BufferInitData* initData, const Dg::BufferDesc& desc);
        void DeclareResource(const ResourceId& id, Rhi::TextureInitData* initData, const Dg::TextureDesc& desc);

    public:
        void ImportBuffer(const ResourceId& id, Dg::IBuffer* buffer);
        void ImportTexture(const ResourceId& id, Dg::ITexture* texture);
        void DiscardResource(const ResourceId& id);

    private:
        void UpdateResources();
        void ClearResources();

        [[nodiscard]] ResourceViewId DeclareBufferView(const ResourceId& viewId, const BufferResourceViewDesc& desc);
        [[nodiscard]] ResourceViewId DeclareTextureView(const ResourceId& viewId, const TextureResourceViewDesc& desc);

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
        void CheckLockState(bool locked) const;

    private:
        [[nodiscard]] bool NeedsRebuild() const noexcept;
        void               SetRebuildState(bool state) noexcept;

    private:
        Rhi::IRhiDevice* m_RhiDevice;
        ResourceMapType  m_Resources;
        UserDataMapType  m_UserDatas;

        bool m_NeedRebuild : 1 = false;
#ifndef AME_DIST
        bool m_Locked : 1 = false;
#endif
    };
} // namespace Ame::Rg