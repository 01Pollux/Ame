#include <Rhi/Device/RhiDevice.hpp>

#include <Shading/Material.CommonState.hpp>
#include <Shading/Hash.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    MaterialCommonState::MaterialCommonState(
        Dg::IRenderDevice*        renderDevice,
        const MaterialCreateDesc& createDesc) :
        m_ResourceSignature(CreatePipelineResourceSignature(renderDevice, createDesc.Properties)),
        m_MaterialDesc(createDesc.Desc),
        m_MaterialHash(CreateBaseMaterialHash(createDesc.Desc))
    {
    }

    //

    const MaterialDesc& MaterialCommonState::GetMaterialDesc() const noexcept
    {
        return m_MaterialDesc;
    }

    auto MaterialCommonState::GetMaterialHash() const noexcept -> MaterialHash
    {
        return m_MaterialHash;
    }

    Dg::IPipelineResourceSignature* MaterialCommonState::GetSignature() const
    {
        return m_ResourceSignature;
    }

    //

    Ptr<Dg::IPipelineResourceSignature> MaterialCommonState::CreatePipelineResourceSignature(
        Dg::IRenderDevice*                       renderDevice,
        const Dg::PipelineResourceSignatureDesc& resourcesDesc)
    {
        Ptr<Dg::IPipelineResourceSignature> resourceSignature;
        if (resourcesDesc.NumResources != 0 ||
            resourcesDesc.NumImmutableSamplers != 0)
        {
            renderDevice->CreatePipelineResourceSignature(resourcesDesc, &resourceSignature);
        }
        return resourceSignature;
    }

    auto MaterialCommonState::CreateBaseMaterialHash(
        const MaterialDesc& materialDesc) -> MaterialHash
    {
        return Dg::ComputeHash(materialDesc);
    }
} // namespace Ame::Rhi