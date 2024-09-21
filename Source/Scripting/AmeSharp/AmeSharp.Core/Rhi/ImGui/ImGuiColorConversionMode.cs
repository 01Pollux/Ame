namespace AmeSharp.Core.Rhi.ImGui;

/// <summary>
/// Conversion mode to apply to imgui colors.
///
/// Imgui colors are defined in sRGB space.
/// Depending on the use case, they may need
/// to be converted to linear space
/// </summary>
public enum ImGuiColorConversionMode : byte
{
    /// Do not perform any color conversion.
    None,

    /// Select the color conversion mode automatically:
    /// * Use SRGB_TO_LINEAR mode for sRGB framebuffers
    /// * Use NONE mode for non-sRGB framebuffers
    Auto,

    /// Always perform srgb-to-linear conversion.
    SrgbToLinear
}
