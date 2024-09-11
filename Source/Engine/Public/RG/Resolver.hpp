#pragma once

#include <Rg/Resource.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Rg
{
    class AME_ENGINE_API Resolver
    {
        friend class PassStorage;

    public:
        Resolver(ResourceStorage& storage);

    public:
        [[nodiscard]] Rhi::IRhiDevice*       GetDevice() const noexcept;
        [[nodiscard]] Dg::TEXTURE_FORMAT     GetBackbufferFormat() const;
        [[nodiscard]] const Dg::TextureDesc& GetBackbufferDesc() const;

    public:
        void ImportBuffer(const ResourceId& id, Dg::IBuffer* buffer);
        void ImportTexture(const ResourceId& id, Dg::ITexture* texture);
        void CreateBuffer(const ResourceId& id, Rhi::BufferInitData* initData, const Dg::BufferDesc& desc);
        void CreateTexture(const ResourceId& id, Rhi::TextureInitData* initData, const Dg::TextureDesc& desc);

    public:
        [[nodiscard]] IObject*                      GetUserData(const ResourceId& id) const;
        template<typename Ty> [[nodiscard]] Ptr<Ty> GetUserData(const ResourceId& id, const UId& iid) const
        {
            auto userdata = GetUserData(id);
            return { userdata, iid };
        }

    public:
        [[nodiscard]] const Dg::BufferDesc&  GetBufferDesc(const ResourceId& id) const;
        [[nodiscard]] const Dg::TextureDesc& GetTextureDesc(const ResourceId& id) const;

    public:
        void           WriteResource(const ResourceId& id);
        void           SetUserData(const ResourceId& id, IObject* userData);
        ResourceViewId WriteBuffer(const ResourceId& id, Dg::BIND_FLAGS bindFlags,
                                   const BufferResourceViewDesc& viewDesc);
        ResourceViewId WriteTexture(const ResourceId& id, Dg::BIND_FLAGS bindFlags,
                                    const TextureResourceViewDesc& viewDesc);

    public:
        void           ReadResource(const ResourceId& id);
        void           ReadUserData(const ResourceId& id);
        ResourceViewId ReadBuffer(const ResourceId& id, Dg::BIND_FLAGS bindFlags,
                                  const BufferResourceViewDesc& viewDesc);
        ResourceViewId ReadTexture(const ResourceId& id, Dg::BIND_FLAGS bindFlags,
                                   const TextureResourceViewDesc& viewDesc);

    private:
        ResourceViewId DeclareView(const ResourceId& id, Dg::BIND_FLAGS bindFlags,
                                   const BufferResourceViewDesc& viewDesc);
        ResourceViewId DeclareView(const ResourceId& id, Dg::BIND_FLAGS bindFlags,
                                   const TextureResourceViewDesc& viewDesc);

    private:
        Ref<ResourceStorage> m_Storage;

        std::set<ResourceId> m_ResourcesRead;
        std::set<ResourceId> m_ResourcesWritten;
    };
} // namespace Ame::Rg