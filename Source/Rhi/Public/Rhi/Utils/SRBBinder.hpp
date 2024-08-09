#pragma once

#include <Rhi/Device/RhiDevice.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/RenderStateCache.hpp>

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
        uint32_t typeFlag = static_cast<uint32_t>(typeFlags);
        uint32_t i        = 0;
        while (typeFlag != 0)
        {
            if (typeFlag & 1)
            {
                auto var = srb->GetVariableByName(static_cast<Dg::SHADER_TYPE>(1 << i), name);
                if (var)
                {
                    var->Set(object, setFlags);
                }
            }
            typeFlag >>= 1;
            ++i;
        }
        srb->GetPipelineResourceSignature();
    }
} // namespace Ame::Rhi