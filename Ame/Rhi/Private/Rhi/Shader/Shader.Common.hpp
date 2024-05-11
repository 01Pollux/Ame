#pragma once

#ifdef AME_PLATFORM_WINDOWS
#include <d3d12shader.h>
#include <dxcapi.h>
#include <wrl.h>

namespace Ame::Rhi::ShaderUtil
{
    template<typename Ty>
    using CComPtr = Microsoft::WRL::ComPtr<Ty>;

    template<typename Ty>
    [[nodiscard]] static inline Ty* GetComPtr(
        CComPtr<Ty>& Object)
    {
        return Object.Get();
    }

    void ThrowShaderException(
        HRESULT Result);
} // namespace Ame::Rhi::ShaderUtil

#else
#include <dxc/dxcapi.h>

namespace Ame::Rhi::ShaderUtil
{
    template<typename Ty>
    [[nodiscard]] static inline Ty* GetComPtr(
        CComPtr<Ty>& Object)
    {
        return Object;
    }

    void ThrowShaderException(
        HRESULT Result);
} // namespace Ame::Rhi::ShaderUtil

#endif
