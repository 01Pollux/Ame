#include <Shading/Material.hpp>

#include <Rhi/Device/RhiDevice.hpp>
#include <Rhi/Utils/SRBBinder.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Material::LocalData::LocalData(
        const MaterialCommonState& commonState,
        const LocalData*           copyFrom) :
        Name(copyFrom ? copyFrom->Name : "")
    {
        auto& userDataDesc = commonState.GetUserDataDesc();
        if (auto userDataSize = userDataDesc.GetStructSize())
        {
            UserDataBuffer = std::make_unique<std::byte[]>(userDataSize);
            if (copyFrom)
            {
                std::memcpy(UserDataBuffer.get(), copyFrom->UserDataBuffer.get(), userDataSize);
            }
        }

        if (auto resourceSignature = commonState.GetSignature())
        {
            resourceSignature->CreateShaderResourceBinding(&Bindings);
            if (copyFrom)
            {
                CopyAllResourcesSrb(copyFrom->Bindings, Bindings);
            }
        }
    }

    Material::SharedData::SharedData(
        Dg::IRenderDevice* renderDevice,
        MaterialCreateDesc createDesc) :
        RenderDevice(renderDevice),
        CommonState(renderDevice, std::move(createDesc))
    {
    }

    //

    Ptr<Material> Material::Create(
        Dg::IRenderDevice* renderDevice,
        MaterialCreateDesc materialDesc)
    {
        return Ptr{ ObjectAllocator<Material>()(renderDevice, materialDesc) };
    }

    //

    Ptr<Material> Material::Instantiate() const
    {
        return Ptr{ ObjectAllocator<Material>()(this) };
    }

    //

    const MaterialShaderSourceStorage& Material::GetMaterialShaders() const noexcept
    {
        return m_SharedData->CommonState.GetMaterialShaders();
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

    void Material::WriteUserData(
        const String&    propertyName,
        const std::byte* data,
        size_t           size)
    {
        auto&    userDataDesc = m_SharedData->CommonState.GetUserDataDesc();
        uint32_t offset       = userDataDesc.GetOffset(propertyName);

#ifdef AME_DEBUG
        Log::Rhi().Assert(offset + size <= GetSizeOfUserData(), "User data buffer overflow");
#endif

        std::memcpy(m_LocalData.UserDataBuffer.get() + offset, data, size);
    }

    void Material::ReadUserData(
        const String& propertyName,
        std::byte*    data,
        size_t        size) const
    {
        auto&    userDataDesc = m_SharedData->CommonState.GetUserDataDesc();
        uint32_t offset       = userDataDesc.GetOffset(propertyName);

#ifdef AME_DEBUG
        Log::Rhi().Assert(offset + size <= GetSizeOfUserData(), "User data buffer overflow");
#endif

        std::memcpy(data, m_LocalData.UserDataBuffer.get() + offset, size);
    }

    const std::byte* Material::GetUserData() const
    {
        return m_LocalData.UserDataBuffer.get();
    }

    uint32_t Material::GetSizeOfUserData() const
    {
        auto& userDataDesc = m_SharedData->CommonState.GetUserDataDesc();
        return userDataDesc.GetStructSize();
    }

    const String& Material::GetName() const
    {
        return m_LocalData.Name;
    }

    void Material::SetName(
        const StringView& name)
    {
        m_LocalData.Name = name;
    }

    //

    Material::Material(
        IReferenceCounters*       counters,
        Dg::IRenderDevice*        renderDevice,
        const MaterialCreateDesc& materialDesc) :
        Base(counters),
        m_SharedData(std::make_shared<SharedData>(renderDevice, materialDesc)),
        m_LocalData(m_SharedData->CommonState, nullptr)
    {
    }

    Material::Material(
        IReferenceCounters* counters,
        const Material*     material) :
        Base(counters),
        m_SharedData(material->m_SharedData),
        m_LocalData(m_SharedData->CommonState, &material->m_LocalData)
    {
    }
} // namespace Ame::Rhi