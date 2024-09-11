#pragma once

#include <Math/Vector.hpp>

namespace Ame::Math
{
    class Color3 : public Vector3
    {
    public:
        using Vector3::Vector3;

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, r);
        AME_VECTOR_IMPL_ACCESSOR(1, g);
        AME_VECTOR_IMPL_ACCESSOR(2, b);
    };

    class Color4 : public Vector4
    {
    public:
        using Vector4::Vector4;

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, r);
        AME_VECTOR_IMPL_ACCESSOR(1, g);
        AME_VECTOR_IMPL_ACCESSOR(2, b);
        AME_VECTOR_IMPL_ACCESSOR(3, a);
    };

    //

    /// <summary>
    /// Converts a Color3 to uint32_t
    /// </summary>
    [[nodiscard]] constexpr uint32_t ColorToU32(const Color3& color)
    {
        return ((static_cast<uint32_t>(color.x() * 255.0f) & 0xFF) << 24) |
               ((static_cast<uint32_t>(color.y() * 255.0f) & 0xFF) << 16) |
               ((static_cast<uint32_t>(color.z() * 255.0f) & 0xFF) << 8) | (0xFF);
    }

    /// <summary>
    /// Converts a Color4 to uint32_t
    /// </summary>
    [[nodiscard]] constexpr uint32_t ColorToU32(const Color4& color)
    {
        return ((static_cast<uint32_t>(color.x() * 255.0f) & 0xFF) << 24) |
               ((static_cast<uint32_t>(color.y() * 255.0f) & 0xFF) << 16) |
               ((static_cast<uint32_t>(color.z() * 255.0f) & 0xFF) << 8) |
               ((static_cast<uint32_t>(color.w() * 255.0f) & 0xFF));
    }

    //

    /// <summary>
    /// Converts a uint32_t to Color3
    /// </summary>
    [[nodiscard]] constexpr Color3 U32ToColor3(uint32_t color)
    {
        return { static_cast<float>((color >> 24) & 0xFF) / 255.0f, static_cast<float>((color >> 16) & 0xFF) / 255.0f,
                 static_cast<float>((color >> 8) & 0xFF) / 255.0f };
    }

    /// <summary>
    /// Converts a uint32_t to Color4
    /// </summary>
    [[nodiscard]] constexpr Color4 U32ToColor4(uint32_t color)
    {
        return { static_cast<float>((color >> 24) & 0xFF) / 255.0f, static_cast<float>((color >> 16) & 0xFF) / 255.0f,
                 static_cast<float>((color >> 8) & 0xFF) / 255.0f, static_cast<float>((color >> 0) & 0xFF) / 255.0f };
    }
} // namespace Ame::Math

namespace Ame::Colors
{
    inline constexpr Math::Color4 c_AliceBlue            = { 0.941176534f, 0.972549081f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_AntiqueWhite         = { 0.980392218f, 0.921568692f, 0.843137324f, 1.000000000f };
    inline constexpr Math::Color4 c_Aqua                 = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Aquamarine           = { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
    inline constexpr Math::Color4 c_Azure                = { 0.941176534f, 1.000000000f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Beige                = { 0.960784376f, 0.960784376f, 0.862745166f, 1.000000000f };
    inline constexpr Math::Color4 c_Bisque               = { 1.000000000f, 0.894117713f, 0.768627524f, 1.000000000f };
    inline constexpr Math::Color4 c_Black                = { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_BlanchedAlmond       = { 1.000000000f, 0.921568692f, 0.803921640f, 1.000000000f };
    inline constexpr Math::Color4 c_Blue                 = { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_BlueViolet           = { 0.541176498f, 0.168627456f, 0.886274576f, 1.000000000f };
    inline constexpr Math::Color4 c_Brown                = { 0.647058845f, 0.164705887f, 0.164705887f, 1.000000000f };
    inline constexpr Math::Color4 c_BurlyWood            = { 0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f };
    inline constexpr Math::Color4 c_CadetBlue            = { 0.372549027f, 0.619607866f, 0.627451003f, 1.000000000f };
    inline constexpr Math::Color4 c_Chartreuse           = { 0.498039246f, 1.000000000f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Chocolate            = { 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f };
    inline constexpr Math::Color4 c_Coral                = { 1.000000000f, 0.498039246f, 0.313725501f, 1.000000000f };
    inline constexpr Math::Color4 c_CornflowerBlue       = { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
    inline constexpr Math::Color4 c_Cornsilk             = { 1.000000000f, 0.972549081f, 0.862745166f, 1.000000000f };
    inline constexpr Math::Color4 c_Crimson              = { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f };
    inline constexpr Math::Color4 c_Cyan                 = { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkBlue             = { 0.000000000f, 0.000000000f, 0.545098066f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkCyan             = { 0.000000000f, 0.545098066f, 0.545098066f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkGoldenrod        = { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkGray             = { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkGreen            = { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkKhaki            = { 0.741176486f, 0.717647076f, 0.419607878f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkMagenta          = { 0.545098066f, 0.000000000f, 0.545098066f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkOliveGreen       = { 0.333333343f, 0.419607878f, 0.184313729f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkOrange           = { 1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkOrchid           = { 0.600000024f, 0.196078449f, 0.800000072f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkRed              = { 0.545098066f, 0.000000000f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkSalmon           = { 0.913725555f, 0.588235319f, 0.478431404f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkSeaGreen         = { 0.560784340f, 0.737254918f, 0.545098066f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkSlateBlue        = { 0.282352954f, 0.239215702f, 0.545098066f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkSlateGray        = { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkTurquoise        = { 0.000000000f, 0.807843208f, 0.819607913f, 1.000000000f };
    inline constexpr Math::Color4 c_DarkViolet           = { 0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f };
    inline constexpr Math::Color4 c_DeepPink             = { 1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f };
    inline constexpr Math::Color4 c_DeepSkyBlue          = { 0.000000000f, 0.749019623f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_DimGray              = { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f };
    inline constexpr Math::Color4 c_DodgerBlue           = { 0.117647067f, 0.564705908f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Firebrick            = { 0.698039234f, 0.133333340f, 0.133333340f, 1.000000000f };
    inline constexpr Math::Color4 c_FloralWhite          = { 1.000000000f, 0.980392218f, 0.941176534f, 1.000000000f };
    inline constexpr Math::Color4 c_ForestGreen          = { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f };
    inline constexpr Math::Color4 c_Fuchsia              = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Gainsboro            = { 0.862745166f, 0.862745166f, 0.862745166f, 1.000000000f };
    inline constexpr Math::Color4 c_GhostWhite           = { 0.972549081f, 0.972549081f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Gold                 = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Goldenrod            = { 0.854902029f, 0.647058845f, 0.125490203f, 1.000000000f };
    inline constexpr Math::Color4 c_Gray                 = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
    inline constexpr Math::Color4 c_Green                = { 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_GreenYellow          = { 0.678431392f, 1.000000000f, 0.184313729f, 1.000000000f };
    inline constexpr Math::Color4 c_Honeydew             = { 0.941176534f, 1.000000000f, 0.941176534f, 1.000000000f };
    inline constexpr Math::Color4 c_HotPink              = { 1.000000000f, 0.411764741f, 0.705882370f, 1.000000000f };
    inline constexpr Math::Color4 c_IndianRed            = { 0.803921640f, 0.360784322f, 0.360784322f, 1.000000000f };
    inline constexpr Math::Color4 c_Indigo               = { 0.294117659f, 0.000000000f, 0.509803951f, 1.000000000f };
    inline constexpr Math::Color4 c_Ivory                = { 1.000000000f, 1.000000000f, 0.941176534f, 1.000000000f };
    inline constexpr Math::Color4 c_Khaki                = { 0.941176534f, 0.901960850f, 0.549019635f, 1.000000000f };
    inline constexpr Math::Color4 c_Lavender             = { 0.901960850f, 0.901960850f, 0.980392218f, 1.000000000f };
    inline constexpr Math::Color4 c_LavenderBlush        = { 1.000000000f, 0.941176534f, 0.960784376f, 1.000000000f };
    inline constexpr Math::Color4 c_LawnGreen            = { 0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_LemonChiffon         = { 1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f };
    inline constexpr Math::Color4 c_LightBlue            = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
    inline constexpr Math::Color4 c_LightCoral           = { 0.941176534f, 0.501960814f, 0.501960814f, 1.000000000f };
    inline constexpr Math::Color4 c_LightCyan            = { 0.878431439f, 1.000000000f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_LightGoldenrodYellow = { 0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f };
    inline constexpr Math::Color4 c_LightGreen           = { 0.564705908f, 0.933333397f, 0.564705908f, 1.000000000f };
    inline constexpr Math::Color4 c_LightGray            = { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f };
    inline constexpr Math::Color4 c_LightPink            = { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f };
    inline constexpr Math::Color4 c_LightSalmon          = { 1.000000000f, 0.627451003f, 0.478431404f, 1.000000000f };
    inline constexpr Math::Color4 c_LightSeaGreen        = { 0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f };
    inline constexpr Math::Color4 c_LightSkyBlue         = { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f };
    inline constexpr Math::Color4 c_LightSlateGray       = { 0.466666698f, 0.533333361f, 0.600000024f, 1.000000000f };
    inline constexpr Math::Color4 c_LightSteelBlue       = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
    inline constexpr Math::Color4 c_LightYellow          = { 1.000000000f, 1.000000000f, 0.878431439f, 1.000000000f };
    inline constexpr Math::Color4 c_Lime                 = { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_LimeGreen            = { 0.196078449f, 0.803921640f, 0.196078449f, 1.000000000f };
    inline constexpr Math::Color4 c_Linen                = { 0.980392218f, 0.941176534f, 0.901960850f, 1.000000000f };
    inline constexpr Math::Color4 c_Magenta              = { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Maroon               = { 0.501960814f, 0.000000000f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumAquamarine     = { 0.400000036f, 0.803921640f, 0.666666687f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumBlue           = { 0.000000000f, 0.000000000f, 0.803921640f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumOrchid         = { 0.729411781f, 0.333333343f, 0.827451050f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumPurple         = { 0.576470613f, 0.439215720f, 0.858823597f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumSeaGreen       = { 0.235294133f, 0.701960802f, 0.443137288f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumSlateBlue      = { 0.482352972f, 0.407843173f, 0.933333397f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumSpringGreen    = { 0.000000000f, 0.980392218f, 0.603921592f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumTurquoise      = { 0.282352954f, 0.819607913f, 0.800000072f, 1.000000000f };
    inline constexpr Math::Color4 c_MediumVioletRed      = { 0.780392230f, 0.082352944f, 0.521568656f, 1.000000000f };
    inline constexpr Math::Color4 c_MidnightBlue         = { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f };
    inline constexpr Math::Color4 c_MintCream            = { 0.960784376f, 1.000000000f, 0.980392218f, 1.000000000f };
    inline constexpr Math::Color4 c_MistyRose            = { 1.000000000f, 0.894117713f, 0.882353008f, 1.000000000f };
    inline constexpr Math::Color4 c_Moccasin             = { 1.000000000f, 0.894117713f, 0.709803939f, 1.000000000f };
    inline constexpr Math::Color4 c_NavajoWhite          = { 1.000000000f, 0.870588303f, 0.678431392f, 1.000000000f };
    inline constexpr Math::Color4 c_Navy                 = { 0.000000000f, 0.000000000f, 0.501960814f, 1.000000000f };
    inline constexpr Math::Color4 c_None                 = { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f };
    inline constexpr Math::Color4 c_OldLace              = { 0.992156923f, 0.960784376f, 0.901960850f, 1.000000000f };
    inline constexpr Math::Color4 c_Olive                = { 0.501960814f, 0.501960814f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_OliveDrab            = { 0.419607878f, 0.556862772f, 0.137254909f, 1.000000000f };
    inline constexpr Math::Color4 c_Orange               = { 1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_OrangeRed            = { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_Orchid               = { 0.854902029f, 0.439215720f, 0.839215755f, 1.000000000f };
    inline constexpr Math::Color4 c_PaleGoldenrod        = { 0.933333397f, 0.909803987f, 0.666666687f, 1.000000000f };
    inline constexpr Math::Color4 c_PaleGreen            = { 0.596078455f, 0.984313786f, 0.596078455f, 1.000000000f };
    inline constexpr Math::Color4 c_PaleTurquoise        = { 0.686274529f, 0.933333397f, 0.933333397f, 1.000000000f };
    inline constexpr Math::Color4 c_PaleVioletRed        = { 0.858823597f, 0.439215720f, 0.576470613f, 1.000000000f };
    inline constexpr Math::Color4 c_PapayaWhip           = { 1.000000000f, 0.937254965f, 0.835294187f, 1.000000000f };
    inline constexpr Math::Color4 c_PeachPuff            = { 1.000000000f, 0.854902029f, 0.725490212f, 1.000000000f };
    inline constexpr Math::Color4 c_Peru                 = { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f };
    inline constexpr Math::Color4 c_Pink                 = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
    inline constexpr Math::Color4 c_Plum                 = { 0.866666734f, 0.627451003f, 0.866666734f, 1.000000000f };
    inline constexpr Math::Color4 c_PowderBlue           = { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f };
    inline constexpr Math::Color4 c_Purple               = { 0.501960814f, 0.000000000f, 0.501960814f, 1.000000000f };
    inline constexpr Math::Color4 c_Red                  = { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_RosyBrown            = { 0.737254918f, 0.560784340f, 0.560784340f, 1.000000000f };
    inline constexpr Math::Color4 c_RoyalBlue            = { 0.254901975f, 0.411764741f, 0.882353008f, 1.000000000f };
    inline constexpr Math::Color4 c_SaddleBrown          = { 0.545098066f, 0.270588249f, 0.074509807f, 1.000000000f };
    inline constexpr Math::Color4 c_Salmon               = { 0.980392218f, 0.501960814f, 0.447058856f, 1.000000000f };
    inline constexpr Math::Color4 c_SandyBrown           = { 0.956862807f, 0.643137276f, 0.376470625f, 1.000000000f };
    inline constexpr Math::Color4 c_SeaGreen             = { 0.180392161f, 0.545098066f, 0.341176480f, 1.000000000f };
    inline constexpr Math::Color4 c_SeaShell             = { 1.000000000f, 0.960784376f, 0.933333397f, 1.000000000f };
    inline constexpr Math::Color4 c_Sienna               = { 0.627451003f, 0.321568638f, 0.176470593f, 1.000000000f };
    inline constexpr Math::Color4 c_Silver               = { 0.752941251f, 0.752941251f, 0.752941251f, 1.000000000f };
    inline constexpr Math::Color4 c_SkyBlue              = { 0.529411793f, 0.807843208f, 0.921568692f, 1.000000000f };
    inline constexpr Math::Color4 c_SlateBlue            = { 0.415686309f, 0.352941185f, 0.803921640f, 1.000000000f };
    inline constexpr Math::Color4 c_SlateGray            = { 0.439215720f, 0.501960814f, 0.564705908f, 1.000000000f };
    inline constexpr Math::Color4 c_Snow                 = { 1.000000000f, 0.980392218f, 0.980392218f, 1.000000000f };
    inline constexpr Math::Color4 c_SpringGreen          = { 0.000000000f, 1.000000000f, 0.498039246f, 1.000000000f };
    inline constexpr Math::Color4 c_SteelBlue            = { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f };
    inline constexpr Math::Color4 c_Tan                  = { 0.823529482f, 0.705882370f, 0.549019635f, 1.000000000f };
    inline constexpr Math::Color4 c_Teal                 = { 0.000000000f, 0.501960814f, 0.501960814f, 1.000000000f };
    inline constexpr Math::Color4 c_Thistle              = { 0.847058892f, 0.749019623f, 0.847058892f, 1.000000000f };
    inline constexpr Math::Color4 c_Tomato               = { 1.000000000f, 0.388235331f, 0.278431386f, 1.000000000f };
    inline constexpr Math::Color4 c_Transparent          = { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f };
    inline constexpr Math::Color4 c_Turquoise            = { 0.250980407f, 0.878431439f, 0.815686345f, 1.000000000f };
    inline constexpr Math::Color4 c_Violet               = { 0.933333397f, 0.509803951f, 0.933333397f, 1.000000000f };
    inline constexpr Math::Color4 c_Wheat                = { 0.960784376f, 0.870588303f, 0.701960802f, 1.000000000f };
    inline constexpr Math::Color4 c_White                = { 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_WhiteSmoke           = { 0.960784376f, 0.960784376f, 0.960784376f, 1.000000000f };
    inline constexpr Math::Color4 c_Yellow               = { 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f };
    inline constexpr Math::Color4 c_YellowGreen          = { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f };
} // namespace Ame::Colors
