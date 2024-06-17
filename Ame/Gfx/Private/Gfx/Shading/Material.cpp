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
        Rhi::Device&              RhiDevice,
        Gfx::Cache::ShaderCache&  ShaderCache,
        Ptr<Rhi::PipelineLayout>  PipelineLayout,
        MaterialPipelineState     PipelineState,
        const PropertyDescriptor& Descriptor) :
        LocalData(Descriptor),
        CommonState(RhiDevice, ShaderCache, PipelineLayout, std::move(PipelineState))
    {
    }

    //

    Material::Material(
        Rhi::Device&              RhiDevice,
        Gfx::Cache::ShaderCache&  ShaderCache,
        Ptr<Rhi::PipelineLayout>  PipelineLayout,
        MaterialPipelineState     PipelineState,
        const PropertyDescriptor& Descriptor) :
        m_SharedData(std::make_shared<SharedData>(RhiDevice, ShaderCache, PipelineLayout, std::move(PipelineState), Descriptor)),
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

    //

    Ptr<Rhi::PipelineLayout> Material::GetPipelineLayout() const
    {
        return m_SharedData->CommonState.GetPipelineLayout();
    }

    Co::result<Ptr<Rhi::PipelineState>> Material::GetPipelineState(
        const MaterialRenderState& RenderState) const
    {
        return m_SharedData->CommonState.GetPipelineState(RenderState);
    }

    //

    const MaterialPipelineState& Material::GetPipelineStateDesc() const
    {
        return m_SharedData->CommonState.GetPipelineStateDesc();
    }

    auto Material::GetPropertyHash() const -> PropertyHash
    {
        if (!m_PropertiesHash)
        {
            UpdatePropertyHash();
        }
        return *m_PropertiesHash;
    }

    auto Material::GetPipelineHash() const -> const PipelineStateHash&
    {
        return m_SharedData->CommonState.GetPipelineHash();
    }
} // namespace Ame::Gfx::Shading