#pragma once

#include <Rhi/Device/RhiDevice.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/RenderStateCache.hpp>

#include <Math/Common.hpp>
#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Bind all set flags in a shader resource binding
    /// this is used for ALL_GRAPHICS flags etc.
    /// </summary>
    static void BindAllInSrb(
        Dg::IShaderResourceBinding*   srb,
        Dg::SHADER_TYPE               typeFlags,
        const char*                   name,
        Dg::IDeviceObject*            object,
        Dg::SET_SHADER_RESOURCE_FLAGS setFlags = Dg::SET_SHADER_RESOURCE_FLAG_NONE)
    {
        while (typeFlags)
        {
            Dg::SHADER_TYPE i   = Math::ExtractLSB(typeFlags);
            auto            var = srb->GetVariableByName(i, name);
            if (var)
            {
                var->Set(object, setFlags);
            }
        }
    }

    static void CopyAllResourcesSrb(
        Dg::IShaderResourceBinding* src,
        Dg::IShaderResourceBinding* dst)
    {
        auto signature = src->GetPipelineResourceSignature();
        Log::Rhi().Assert(signature->IsCompatibleWith(dst->GetPipelineResourceSignature()), "Incompatible resource signatures");
        auto& signatureDesc = signature->GetDesc();

        std::vector<Dg::IDeviceObject*> objects;
        for (auto& resource : std::span{ signatureDesc.Resources, signatureDesc.NumResources })
        {
            Dg::SHADER_TYPE typeFlags = resource.ShaderStages;
            while (typeFlags)
            {
                Dg::SHADER_TYPE i = Math::ExtractLSB(typeFlags);

                auto srcVar = src->GetVariableByName(i, resource.Name);
                auto dstVar = dst->GetVariableByName(i, resource.Name);

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