#pragma once

#include <Rhi/Device/RhiDevice.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/RenderStateCache.hpp>

#include <Math/Common.hpp>
#include <Log/Logger.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Bind all set flags in a shader resource binding
    /// this is used for ALL_GRAPHICS flags etc.
    /// </summary>
    inline void BindAllStaticInSignature(Dg::IPipelineResourceSignature* signature, Dg::SHADER_TYPE typeFlags,
                                         const char* name, Dg::IDeviceObject* object,
                                         Dg::SET_SHADER_RESOURCE_FLAGS setFlags = Dg::SET_SHADER_RESOURCE_FLAG_NONE)
    {
        while (typeFlags)
        {
            Dg::SHADER_TYPE curType = Math::ExtractLSB(typeFlags);
            auto            var     = signature->GetStaticVariableByName(curType, name);
            if (var)
            {
                var->Set(object, setFlags);
            }
        }
    }

    /// <summary>
    /// Bind all set flags in a shader resource binding
    /// this is used for ALL_GRAPHICS flags etc.
    /// </summary>
    inline void BindAllStaticInSrb(Dg::IShaderResourceBinding* srb, Dg::SHADER_TYPE typeFlags, const char* name,
                                   Dg::IDeviceObject*            object,
                                   Dg::SET_SHADER_RESOURCE_FLAGS setFlags = Dg::SET_SHADER_RESOURCE_FLAG_NONE)
    {
        auto signature = srb->GetPipelineResourceSignature();
        BindAllStaticInSignature(signature, typeFlags, name, object, setFlags);
    }

    /// <summary>
    /// Bind all set flags in a shader resource binding
    /// this is used for ALL_GRAPHICS flags etc.
    /// </summary>
    inline void BindAllInSrb(Dg::IShaderResourceBinding* srb, Dg::SHADER_TYPE typeFlags, const char* name,
                             Dg::IDeviceObject*            object,
                             Dg::SET_SHADER_RESOURCE_FLAGS setFlags = Dg::SET_SHADER_RESOURCE_FLAG_NONE)
    {
        while (typeFlags)
        {
            Dg::SHADER_TYPE curType = Math::ExtractLSB(typeFlags);
            auto            var     = srb->GetVariableByName(curType, name);
            if (var)
            {
                var->Set(object, setFlags);
            }
        }
    }

    /// <summary>
    /// Copy all resources from one SRB to another
    /// </summary>
    inline void CopyAllResourcesSrb(Dg::IShaderResourceBinding* src, Dg::IShaderResourceBinding* dst)
    {
        auto signature = src->GetPipelineResourceSignature();
        AME_LOG_ASSERT(
            signature->IsCompatibleWith(dst->GetPipelineResourceSignature()), "Incompatible resource signatures");
        auto& signatureDesc = signature->GetDesc();

        std::vector<Dg::IDeviceObject*> objects;
        for (auto& resource : std::span{ signatureDesc.Resources, signatureDesc.NumResources })
        {
            Dg::SHADER_TYPE typeFlags = resource.ShaderStages;
            while (typeFlags)
            {
                Dg::SHADER_TYPE curType = Math::ExtractLSB(typeFlags);

                auto srcVar = src->GetVariableByName(curType, resource.Name);
                auto dstVar = dst->GetVariableByName(curType, resource.Name);

                if (dstVar && srcVar)
                {
                    objects.reserve(resource.ArraySize);
                    objects.clear();
                    for (uint32_t i = 0; i < resource.ArraySize; i++)
                    {
                        objects.emplace_back(srcVar->Get(i));
                    }
                    dstVar->SetArray(objects.data(), 0, resource.ArraySize);
                }
            }
        }
    }
} // namespace Ame::Rhi