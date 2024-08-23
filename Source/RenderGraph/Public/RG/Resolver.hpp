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
            ResourceStorage&      storage,
            ResourceSynchronizer& synchronizer);

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
        [[nodiscard]] Co::result<Dg::IBuffer*>  GetBuffer(ResourceId id) const;
        [[nodiscard]] Co::result<Dg::ITexture*> GetTexture(ResourceId id) const;

        [[nodiscard]] Co::result<IObject*> GetUserData(ResourceId id) const;
        template<typename Ty>
        [[nodiscard]] Co::result<Ptr<Ty>> GetUserData(ResourceId id, const UId& iid) const
        {
            auto userdata = co_await GetUserData(id);
            co_return { userdata, iid };
        }

    public:
        [[nodiscard]] Co::result<void>              WriteResource(ResourceId id);
        [[nodiscard]] void                          SetUserData(ResourceId id, IObject* userData);
        [[nodiscard]] Co::result<Dg::IBuffer*>      WriteBuffer(ResourceId id);
        [[nodiscard]] Co::result<Dg::IBufferView*>  WriteBuffer(ResourceId id, const BufferResourceViewDesc& viewDesc);
        [[nodiscard]] Co::result<Dg::ITexture*>     WriteTexture(ResourceId id);
        [[nodiscard]] Co::result<Dg::ITextureView*> WriteTexture(ResourceId id, const TextureResourceViewDesc& viewDesc);

    public:
        [[nodiscard]] Co::result<void>              ReadResource(ResourceId id);
        [[nodiscard]] Co::result<IObject*>          ReadUserData(ResourceId id);
        [[nodiscard]] Co::result<Dg::IBuffer*>      ReadBuffer(ResourceId id);
        [[nodiscard]] Co::result<Dg::IBufferView*>  ReadBuffer(ResourceId id, const BufferResourceViewDesc& viewDesc);
        [[nodiscard]] Co::result<Dg::ITexture*>     ReadTexture(ResourceId id);
        [[nodiscard]] Co::result<Dg::ITextureView*> ReadTexture(ResourceId id, const TextureResourceViewDesc& viewDesc);

    private:
        Ref<ResourceStorage>      m_Storage;
        Ref<ResourceSynchronizer> m_Synchronizer;

        std::set<ResourceId> m_ResourcesRead;
        std::set<ResourceId> m_ResourcesWritten;
    };
} // namespace Ame::Rg