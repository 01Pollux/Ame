#pragma once

#include <Shading/Core.hpp>

namespace Ame::Rhi
{
    class MaterialCommonState
    {
    public:
        MaterialCommonState(
            Dg::IRenderDevice* renderDevice,
            MaterialCreateDesc createDesc);

    public:
        /// <summary>
        /// Get the user data descriptor for the material
        /// </summary>
        [[nodiscard]] const MaterialPropertyDescriptor& GetUserDataDesc() const noexcept;

        /// <summary>
        /// Get the shader sources for the material
        /// </summary>
        [[nodiscard]] const MaterialShaderSourceStorage& GetMaterialShaders() const noexcept;

        /// <summary>
        /// Get the material hash for the base material desc
        /// </summary>
        [[nodiscard]] MaterialHash GetMaterialHash() const noexcept;

        /// <summary>
        /// Get the resource signature for the material
        /// </summary>
        [[nodiscard]] Dg::IPipelineResourceSignature* GetSignature() const;

    private:
        /// <summary>
        /// Create resource signature for material desc
        /// </summary>
        [[nodiscard]] static Ptr<Dg::IPipelineResourceSignature> CreatePipelineResourceSignature(
            Dg::IRenderDevice*                       renderDevice,
            const Dg::PipelineResourceSignatureDesc& resourcesDesc);

        /// <summary>
        /// Create material's hash
        /// </summary>
        [[nodiscard]] static MaterialHash CreateBaseMaterialHash(
            const MaterialShaderSourceStorage& materialDesc);

    private:
        Ptr<Dg::IPipelineResourceSignature> m_ResourceSignature;
        MaterialPropertyDescriptor          m_UserDataDesc;

        MaterialShaderSourceStorage m_MaterialShaders;
        MaterialHash                m_MaterialHash;
    };
} // namespace Ame::Rhi