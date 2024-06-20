#pragma once

#ifdef AME_PLATFORM_WINDOWS
struct AGSContext;

#include <stdexcept>

#include <Core/String.hpp>
#include <NRI/NRI.h>
#include <NRI/Extensions/NRIWrapperD3D12.h>

#include <d3d12sdklayers.h>
#include <dxgi.h>
#include <wrl/client.h>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::D3D12
{
    template<typename... ArgsTy>
    void ThrowIfHrFailed(
        HRESULT                             hr,
        const std::format_string<ArgsTy...> message,
        ArgsTy&&... args)
    {
        if (FAILED(hr))
        {
            throw std::runtime_error(std::format(std::move(message), std::forward<ArgsTy>(args)...));
        }
    }

    template<typename... ArgsTy>
    void LogAndThrowIfHrFailed(
        HRESULT                             hr,
        const std::format_string<ArgsTy...> message,
        ArgsTy&&... args)
    {
#ifndef AME_DIST
        Log::Rhi().Assert(SUCCEEDED(hr), std::move(message), std::forward<ArgsTy>(args)...);
#else
        if (FAILED(hr))
        {
            Log::Rhi().Error(std::move(message), std::forward<ArgsTy>(args)...);
        }
#endif
    }
} // namespace Ame::Rhi::D3D12
#endif
