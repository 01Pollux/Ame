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
        Rhi::DeviceResourceAllocator&  allocator,
        Gfx::Cache::ShaderCache&       ShaderCache,
        Cache::CommonPipelineState&    pipelineStateCache,
        Ptr<Rhi::ScopedPipelineLayout> pipelineLayout,
        MaterialPipelineState          pipelineState,
        const PropertyDescriptor&      descriptor) :
        LocalData(descriptor),
        CommonState(allocator, ShaderCache, pipelineStateCache, std::move(pipelineLayout), std::move(pipelineState))
    {
    }

    //

    Material::Material(
        Rhi::DeviceResourceAllocator&  allocator,
        Gfx::Cache::ShaderCache&       shaderCache,
        Cache::CommonPipelineState&    pipelineStateCache,
        Ptr<Rhi::ScopedPipelineLayout> pipelineLayout,
        MaterialPipelineState          pipelineState,
        const PropertyDescriptor&      descriptor) :
        m_SharedData(std::make_shared<SharedData>(allocator, shaderCache, pipelineStateCache, std::move(pipelineLayout), std::move(pipelineState), descriptor)),
        m_LocalData(descriptor)
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

    Ptr<Rhi::ScopedPipelineLayout> Material::GetPipelineLayout() const
    {
        return m_SharedData->CommonState.GetPipelineLayout();
    }

    Co::result<Ptr<Rhi::ScopedPipelineState>> Material::GetPipelineState(
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