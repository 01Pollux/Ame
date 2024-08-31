#include <Rhi/Device/RhiDevice.hpp>

#include <Shading/Material.CommonState.hpp>
#include <Shading/Hash.hpp>

namespace Ame::Rhi
{
    MaterialCommonState::MaterialCommonState(Dg::IRenderDevice* renderDevice, MaterialCreateDesc createDesc) :
        m_ResourceSignature(CreatePipelineResourceSignature(renderDevice, std::move(createDesc.ResourceSignature))),
        m_UserDataDesc(std::move(createDesc.UserData)), m_MaterialShaders(std::move(createDesc.Shaders)),
        m_MaterialHash(CreateBaseMaterialHash(m_MaterialShaders))
    {
    }

    //

    const MaterialPropertyDescriptor& MaterialCommonState::GetUserDataDesc() const noexcept
    {
        return m_UserDataDesc;
    }

    const MaterialShaderSourceStorage& MaterialCommonState::GetMaterialShaders() const noexcept
    {
        return m_MaterialShaders;
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
        Dg::IRenderDevice* renderDevice, const Dg::PipelineResourceSignatureDesc& resourcesDesc)
    {
        Ptr<Dg::IPipelineResourceSignature> resourceSignature;
        if (resourcesDesc.NumResources != 0 || resourcesDesc.NumImmutableSamplers != 0)
        {
            renderDevice->CreatePipelineResourceSignature(resourcesDesc, &resourceSignature);
        }
        return resourceSignature;
    }

    auto MaterialCommonState::CreateBaseMaterialHash(const MaterialShaderSourceStorage& shaders) -> MaterialHash
    {
        return Dg::ComputeHash(shaders);
    }
} // namespace Ame::Rhi