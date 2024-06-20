#pragma once

#include <stdexcept>

#include <Core/String.hpp>
#include <NRI/NRI.h>
#include <NRI/Extensions/NRIWrapperVK.h>


#if defined(AME_PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(AME_PLATFORM_LINUX)
#define VK_USE_PLATFORM_XLIB_KHR
#else
#error "Unsupported Platform for vulkan"
#endif

#define VK_NO_PROTOTYPES 1
#include <vulkan/vulkan.h>
#include <Core/Enum.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::VK
{
    template<typename... ArgsTy>
    void ThrowIfVkFailed(
        VkResult                            result,
        const std::format_string<ArgsTy...> message,
        ArgsTy&&... args)
    {
        if (result != VK_SUCCESS)
        {
            String message = std::format("[Vulkan error: {0}] {1}", Enum::enum_name(result), message, std::forward<ArgsTy>(args)...);
            throw std::runtime_error(message);
        }
    }

    template<typename... ArgsTy>
    void LogAndThrowIfVkFailed(
        VkResult                            result,
        const std::format_string<ArgsTy...> message,
        ArgsTy&&... args)
    {
        if (result != VK_SUCCESS)
        {
            String message = std::format("[Vulkan error: {0}] {1}", Enum::enum_name(result), message, std::forward<ArgsTy>(args)...);
#ifndef AME_DIST
            Log::Rhi().Assert(false, std::move(message));
#else
            if (FAILED(hr))
            {
                Log::Rhi().Error(std::move(message));
            }
#endif
        }
    }
} // namespace Ame::Rhi::VK
