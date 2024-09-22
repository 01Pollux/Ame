namespace AmeSharp.Rhi.RhiDevice;

[Flags]
public enum RhiDeviceValidationType : uint
{
    None = 0,

    /// Enable backend-specific validation (e.g. use Direct3D11 debug device, enable Direct3D12
    /// debug layer, enable Vulkan validation layers, create debug OpenGL context, etc.).
    /// The validation is enabled by default in Debug/Development builds and disabled
    /// in release builds.
    DebugLayer = 0x01,

    /// Verify that constant or structured buffer size is not smaller than what is expected by the shader.
    ///
    /// \remarks  This flag only has effect in Debug/Development builds.
    ///           This type of validation is never performed in Release builds.
    ///
    /// \note   This option is currently supported by Vulkan backend only.
    CheckShaderBufferSize = 0x01,

    /// D3D11:
    /// Verify that all committed context resources are relevant,
    /// i.e. they are consistent with the committed resource cache.
    /// This is very expensive and should only be used for engine debugging.
    /// This option is enabled in validation level 2 (see Diligent::VALIDATION_LEVEL).
    ///
    /// \remarks  This flag only has effect in Debug/Development builds.
    ///           This type of validation is never performed in Release builds.
    CommittedResourceRelevance = 1 << 0,

    /// D3D12:
    /// Whether to break execution when D3D12 debug layer detects an error.
    /// This flag only has effect if validation is enabled (EngineCreateInfo.EnableValidation is true).
    /// This option is disabled by default in all validation levels.
    BreakOnError = 1 << 1,

    /// D3D12:
    /// Whether to break execution when D3D12 debug layer detects a memory corruption.
    /// This flag only has effect if validation is enabled (EngineCreateInfo.EnableValidation is true).
    /// This option is enabled by default when validation is enabled.
    BreakOnCorruption = 1 << 2,

    /// D3D12:
    /// Enable validation on the GPU timeline.
    /// See https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-d3d12-debug-layer-gpu-based-validation
    /// This flag only has effect if validation is enabled (EngineCreateInfo.EnableValidation is true).
    /// This option is enabled in validation level 2 (see Diligent::VALIDATION_LEVEL).
    ///
    /// \note Enabling this option may slow things down a lot.
    GpuBasedValidation = 1 << 3,

    All = DebugLayer | CheckShaderBufferSize | CommittedResourceRelevance | BreakOnError | BreakOnCorruption |
          GpuBasedValidation
}
