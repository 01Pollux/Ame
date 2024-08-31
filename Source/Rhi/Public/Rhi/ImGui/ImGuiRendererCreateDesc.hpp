#pragma once

#include <Core/Interface.hpp>
#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    /// Conversion mode to apply to imgui colors.
    ///
    /// \remarks    Imgui colors are defined in sRGB space.
    ///             Depending on the use case, they may need
    ///             to be converted to linear space.
    enum class ImGuiColorConversionMode : uint8_t
    {
        /// Do not perform any color conversion.
        None,

        /// Select the color conversion mode automatically:
        /// * Use SRGB_TO_LINEAR mode for sRGB framebuffers
        /// * Use NONE mode for non-sRGB framebuffers
        Auto,

        /// Always perform srgb-to-linear conversion.
        SrgbToLinear
    };

    struct ImGuiRendererCreateDesc
    {
        IRhiDevice* RhiDevice = nullptr;

        uint32_t InitialVertexBufferSize = 0;
        uint32_t InitialIndexBufferSize  = 0;

        ImGuiColorConversionMode ConversionMode = ImGuiColorConversionMode::Auto;

        /// <summary>
        /// If true, the renderer will be created in a multi-threaded mode, and only one instance is allowed to execute
        /// at a time. (Between BeginFrame and EndFrame)
        /// </summary>
        bool MultiThreaded : 1 = true;
    };
} // namespace Ame::Rhi