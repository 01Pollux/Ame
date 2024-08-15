#pragma once

#include <Shading/Core.hpp>

namespace Ame::Rhi
{
    class MaterialCommonState
    {
    public:
        static constexpr Dg::SHADER_TYPE c_AllSupportedShaders[]{
            Dg::SHADER_TYPE_VERTEX,
            Dg::SHADER_TYPE_PIXEL,
            Dg::SHADER_TYPE_GEOMETRY,
            Dg::SHADER_TYPE_HULL,
            Dg::SHADER_TYPE_DOMAIN
        };

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
        /// Get the base material desc for the material
        /// </summary>
        [[nodiscard]] const MaterialDesc& GetMaterialDesc() const noexcept;

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
            const MaterialDesc& materialDesc);

    private:
        Ptr<Dg::IPipelineResourceSignature> m_ResourceSignature;
        MaterialPropertyDescriptor          m_UserDataDesc;

        MaterialDesc m_MaterialDesc;
        MaterialHash m_MaterialHash;
    };
} // namespace Ame::Rhi