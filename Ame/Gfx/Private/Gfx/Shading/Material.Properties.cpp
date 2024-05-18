#include <Gfx/Shading/Material.hpp>
#include <Rhi/Device/Device.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    void Material::MakeLocal(
        const String& PropertyName,
        bool          Local)
    {
        m_PropertyLocalMap[GetPropertyRootPath(PropertyName)] = Local;
    }

    bool Material::IsLocal(
        const String& Property) const
    {
        auto RootPath = GetPropertyRootPath(Property);
        auto Iter     = m_PropertyLocalMap.find(RootPath);
        return Iter != m_PropertyLocalMap.end() ? Iter->second : false;
    }

    //

    void Material::Set(
        const String&            Property,
        const Ptr<Rhi::Texture>& Texture,
        Rhi::TextureViewDesc     ViewDesc)
    {
        InvalidateHash();
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.WriteTexture(Property, Texture, std::move(ViewDesc));
    }

    void Material::Set(
        const String&           Property,
        const Ptr<Rhi::Buffer>& Buffer,
        Rhi::BufferViewDesc     ViewDesc)
    {
        InvalidateHash();
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.WriteBuffer(Property, Buffer, std::move(ViewDesc));
    }

    void Material::Set(
        const String&    Property,
        Rhi::SamplerDesc SamplerDesc)
    {
        InvalidateHash();
        auto&                    Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Rhi::SamplerResourceView Sampler(m_SharedData->CommonState.GetDevice(), SamplerDesc);
        Properties.WriteSampler(Property, std::move(Sampler), std::move(SamplerDesc));
    }

    const TextureResource& Material::GetTexture(
        const String& Property) const
    {
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        return Properties.ReadTexture(Property);
    }

    const BufferResource& Material::GetBuffer(
        const String& Property) const
    {
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        return Properties.ReadBuffer(Property);
    }

    const SamplerResource& Material::GetSampler(
        const String& Property) const
    {
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        return Properties.ReadSampler(Property);
    }

    //

    void Material::SetScalar(
        const String& Property,
        const void*   Value,
        size_t        Size)
    {
        InvalidateHash();
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.WriteUserData(Property, Value, Size);
    }

    void Material::GetScalar(
        const String& Property,
        void*         Value,
        size_t        Size) const
    {
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.ReadUserData(Property, Value, Size);
    }

    //

    String Material::GetPropertyRootPath(
        const String& Path)
    {
        auto First = Path.find_first_of('.');
        if (First == String::npos)
        {
            return Path;
        }
        else
        {
            return Path.substr(0, First);
        }
    }
} // namespace Ame::Gfx::Shading