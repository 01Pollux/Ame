#include <Gfx/Shading/Material.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    Material::LocalData::LocalData(
        const PropertyDescriptor& Descriptor) :
        Properties(Descriptor)
    {
    }

    Material::LocalData::LocalData(
        const PropertyMap& Properties) :
        Properties(&Properties)
    {
    }

    //

    Material::SharedData::SharedData(
        Rhi::Device&                 RhiDevice,
        Ptr<Rhi::PipelineLayout>     PipelineLayout,
        const MaterialPipelineState& PipelineState,
        const PropertyDescriptor&    Descriptor) :
        LocalData(Descriptor),
        CommonState(RhiDevice, PipelineLayout, PipelineState)
    {
    }

    //

    Material::Material(
        Rhi::Device&                 RhiDevice,
        Ptr<Rhi::PipelineLayout>     PipelineLayout,
        const MaterialPipelineState& PipelineState,
        const PropertyDescriptor&    Descriptor) :
        m_SharedData(std::make_shared<SharedData>(RhiDevice, PipelineLayout, PipelineState, Descriptor)),
        m_LocalData(Descriptor)
    {
    }

    Material::Material(
        const Material* Mat) :
        m_SharedData(Mat->m_SharedData),
        m_LocalData(Mat->m_LocalData.Properties)
    {
    }

    //

    Ptr<Material> Material::Instantiate() const
    {
        return std::make_shared<Material>(this);
    }

    Co::result<Ptr<Rhi::PipelineState>> Material::GetPipelineState(
        const MaterialRenderState& RenderState) const
    {
        return m_SharedData->CommonState.GetPipelineState(RenderState);
    }

    //

    auto Material::GetHash() const -> PropertyHash
    {
        if (!m_PropertiesHash)
        {
            UpdateHash();
        }
        return *m_PropertiesHash;
    }

    uint32_t Material::GetSizeOfUserData() const
    {
        return m_SharedData->Properties.GetSizeOfUserData();
    }
} // namespace Ame::Gfx::Shading