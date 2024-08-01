#include <Shading/Material.hpp>

#include <Rhi/Device/RhiDevice.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Material::LocalData::LocalData(
        Dg::IRenderDevice*         renderDevice,
        const MaterialCommonState& commonState)
    {
        auto resourceSignature = commonState.GetSignature();
        resourceSignature->CreateShaderResourceBinding(&Bindings);

        auto& materialDesc = commonState.GetMaterialDesc();
        if (materialDesc.UserDataSize > 0)
        {
#ifndef AME_DIST
            String      bufferName    = std::format("UD_{}", materialDesc.Name);
            const char* bufferNamePtr = bufferName.c_str();
#else
            const char* bufferNamePtr = nullptr;
#endif
            Dg::CreateUniformBuffer(
                renderDevice,
                materialDesc.UserDataSize,
                bufferNamePtr,
                &UserDataBuffer,
                Dg::USAGE_DYNAMIC,
                Dg::BIND_UNIFORM_BUFFER,
                Dg::CPU_ACCESS_WRITE);

            bool userDataFound = false;
            for (auto shaderType : MaterialCommonState::c_AllSupportedShaders)
            {
                auto variable = Bindings->GetVariableByName(shaderType, UserDataPropertyTag);
                if (variable)
                {
                    variable->Set(UserDataBuffer, Dg::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE);
                    userDataFound = true;
                }
            }

#ifndef AME_DIST
            if (!userDataFound)
            {
                Log::Gfx().Warning("User data property not found in material '{}'", materialDesc.Name);
            }
#endif
        }
    }

    Material::SharedData::SharedData(
        Dg::IRenderDevice*        renderDevice,
        const MaterialCreateDesc& createDesc) :
        RenderDevice(renderDevice),
        CommonState(renderDevice, createDesc)
    {
    }

    //

    Ptr<Material> Material::Create(
        Dg::IRenderDevice*        renderDevice,
        const MaterialCreateDesc& materialDesc)
    {
        return Ptr{ ObjectAllocator<Material>()(renderDevice, materialDesc) };
    }

    //

    Ptr<Material> Material::Instantiate() const
    {
        return Ptr{ ObjectAllocator<Material>()(this) };
    }

    //

    const MaterialDesc& Material::GetMaterialDesc() const noexcept
    {
        return m_SharedData->CommonState.GetMaterialDesc();
    }

    auto Material::GetMaterialHash() const noexcept -> MaterialHash
    {
        return m_SharedData->CommonState.GetMaterialHash();
    }

    Dg::IShaderResourceBinding* Material::GetBindings() const
    {
        return m_LocalData.Bindings;
    }

    Dg::IPipelineResourceSignature* Material::GetResourceSignature() const
    {
        return m_SharedData->CommonState.GetSignature();
    }

    //

    uint32_t Material::GetSizeOfUserData() const
    {
        return m_SharedData->CommonState.GetMaterialDesc().UserDataSize;
    }

    std::byte* Material::MapUserData(
        Dg::IDeviceContext* deviceContext,
        Dg::MAP_FLAGS       mapFlags)
    {
        Dg::PVoid userData = nullptr;
        deviceContext->MapBuffer(m_LocalData.UserDataBuffer, Dg::MAP_WRITE, mapFlags, userData);
        return std::bit_cast<std::byte*>(userData);
    }

    void Material::UnmapUserData(
        Dg::IDeviceContext* deviceContext)
    {
        deviceContext->UnmapBuffer(m_LocalData.UserDataBuffer, Dg::MAP_WRITE);
    }

    //

    Material::Material(
        IReferenceCounters*       counters,
        Dg::IRenderDevice*        renderDevice,
        const MaterialCreateDesc& materialDesc) :
        Base(counters),
        m_SharedData(std::make_shared<SharedData>(renderDevice, materialDesc)),
        m_LocalData(renderDevice, m_SharedData->CommonState)
    {
    }

    Material::Material(
        IReferenceCounters* counters,
        const Material*     material) :
        Base(counters),
        m_SharedData(material->m_SharedData),
        m_LocalData(material->m_SharedData->RenderDevice, material->m_SharedData->CommonState)
    {
    }
} // namespace Ame::Rhi