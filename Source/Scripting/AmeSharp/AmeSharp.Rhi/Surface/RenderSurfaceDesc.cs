using Diligent;

namespace AmeSharp.Rhi.Surface;

public struct FullscreenModeDesc()
{
    public uint RefreshRateNumerator = 0;
    public uint RefreshRateDenominator = 0;

    public ScalingMode Scaling = ScalingMode.Unspecified;
    public ScanlineOrder ScanlineOrder = ScanlineOrder.Unspecified;
    public bool Fullscreen = false;
};

public struct SwapchainDesc()
{
    /// <summary>
    /// Back buffer format. Default value is Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB
    /// </summary>
    public TextureFormat ColorFormat = TextureFormat.RGBA8_UNorm_sRGB;

    /// <summary>
    /// Depth buffer format. Default value is Diligent::TEX_FORMAT_D32_FLOAT.
    /// Use Diligent::TEX_FORMAT_UNKNOWN to create the swap chain without depth buffer.
    /// </summary>
    public TextureFormat DepthFormat = TextureFormat.Unknown;

    /// <summary>
    /// Swap chain usage flags. Default value is Diligent::SWAP_CHAIN_USAGE_RENDER_TARGET
    /// </summary>
    public SwapChainUsageFlags Usage = SwapChainUsageFlags.RenderTarget;

    /// <summary>
    /// The transform, relative to the presentation engine's natural orientation,
    /// applied to the image content prior to presentation.
    ///
    /// \note When default value (SURFACE_TRANSFORM_OPTIMAL) is used, the engine will
    ///       select the most optimal surface transformation. An application may request
    ///       specific transform (e.g. SURFACE_TRANSFORM_IDENTITY) and the engine will
    ///       try to use that. However, if the transform is not available, the engine will
    ///       select the most optimal transform.
    ///       After the swap chain has been created, this member will contain the actual
    ///       transform selected by the engine and can be queried through ISwapChain::GetDesc()
    ///       method.
    /// </summary>
    public SurfaceTransform PreTransform = SurfaceTransform.Optimal;

    /// <summary>
    /// The number of buffers in the swap chain
    /// </summary>
    public uint BufferCount = 0; // 0 = Use whatever diligent default deems default.

    /// <summary>
    /// Default depth value, which is used as optimized depth clear value in D3D12
    /// </summary>
    public float DefaultDepthValue = 1.0f;

    /// <summary>
    /// Default stencil value, which is used as optimized stencil clear value in D3D12
    /// </summary>
    public byte DefaultStencilValue = 0;

    /// <summary>
    /// Indicates if this is a primary swap chain. When Present() is called
    /// for the primary swap chain, the engine releases stale resources.
    /// </summary>
    public bool IsPrimary = true;
};

public sealed record RenderSurfaceDesc()
{
    public required IWindow Window;
    public FullscreenModeDesc Fullscreen = new();
    public SwapchainDesc Swapchain = new();
}
