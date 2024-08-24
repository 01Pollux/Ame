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
        [[nodiscard]] Dg::IBuffer*  GetBuffer(const ResourceId& id) const;
        [[nodiscard]] Dg::ITexture* GetTexture(const ResourceId& id) const;

        [[nodiscard]] IObject* GetUserData(const ResourceId& id) const;
        template<typename Ty>
        [[nodiscard]] Ptr<Ty> GetUserData(const ResourceId& id, const UId& iid) const
        {
            auto userdata = co_await GetUserData(id);
            co_return { userdata, iid };
        }

    public:
        void              WriteResource(const ResourceId& id);
        void              SetUserData(const ResourceId& id, IObject* userData);
        Dg::IBuffer*      WriteBuffer(const ResourceId& id);
        Dg::IBufferView*  WriteBuffer(const ResourceId& id, const BufferResourceViewDesc& viewDesc);
        Dg::ITexture*     WriteTexture(const ResourceId& id);
        Dg::ITextureView* WriteTexture(const ResourceId& id, const TextureResourceViewDesc& viewDesc);

    public:
        void              ReadResource(const ResourceId& id);
        IObject*          ReadUserData(const ResourceId& id);
        Dg::IBuffer*      ReadBuffer(const ResourceId& id);
        Dg::IBufferView*  ReadBuffer(const ResourceId& id, const BufferResourceViewDesc& viewDesc);
        Dg::ITexture*     ReadTexture(const ResourceId& id);
        Dg::ITextureView* ReadTexture(const ResourceId& id, const TextureResourceViewDesc& viewDesc);

    private:
        Ref<ResourceStorage> m_Storage;

        std::set<ResourceId> m_ResourcesRead;
        std::set<ResourceId> m_ResourcesWritten;
    };
} // namespace Ame::Rg