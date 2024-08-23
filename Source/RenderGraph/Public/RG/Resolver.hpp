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
            ResourceStorage& storage);

    public:
        [[nodiscard]] Rhi::IRhiDevice*       GetDevice() const noexcept;
        [[nodiscard]] Dg::TEXTURE_FORMAT     GetBackbufferFormat() const;
        [[nodiscard]] const Dg::TextureDesc& GetBackbufferDesc() const;

    public:
        void          ImportBuffer(const ResourceId& id, Dg::IBuffer* buffer);
        void          ImportTexture(const ResourceId& id, Dg::ITexture* texture);
        Dg::IBuffer*  CreateBuffer(const ResourceId& id, const Dg::BufferData* initData, const Dg::BufferDesc& desc);
        Dg::ITexture* CreateTexture(const ResourceId& id, const Dg::TextureData* initData, const Dg::TextureDesc& desc);

    public:
        [[nodiscard]] Dg::IBuffer*  GetBuffer(ResourceId id) const;
        [[nodiscard]] Dg::ITexture* GetTexture(ResourceId id) const;

        [[nodiscard]] IObject* GetUserData(ResourceId id) const;
        template<typename Ty>
        [[nodiscard]] Ptr<Ty> GetUserData(ResourceId id, const UId& iid) const
        {
            auto userdata = co_await GetUserData(id);
            co_return { userdata, iid };
        }

    public:
        void              WriteResource(ResourceId id);
        void              SetUserData(ResourceId id, IObject* userData);
        Dg::IBuffer*      WriteBuffer(ResourceId id);
        Dg::IBufferView*  WriteBuffer(ResourceId id, const BufferResourceViewDesc& viewDesc);
        Dg::ITexture*     WriteTexture(ResourceId id);
        Dg::ITextureView* WriteTexture(ResourceId id, const TextureResourceViewDesc& viewDesc);

    public:
        void              ReadResource(ResourceId id);
        IObject*          ReadUserData(ResourceId id);
        Dg::IBuffer*      ReadBuffer(ResourceId id);
        Dg::IBufferView*  ReadBuffer(ResourceId id, const BufferResourceViewDesc& viewDesc);
        Dg::ITexture*     ReadTexture(ResourceId id);
        Dg::ITextureView* ReadTexture(ResourceId id, const TextureResourceViewDesc& viewDesc);

    private:
        Ref<ResourceStorage> m_Storage;

        std::set<ResourceId> m_ResourcesRead;
        std::set<ResourceId> m_ResourcesWritten;
    };
} // namespace Ame::Rg