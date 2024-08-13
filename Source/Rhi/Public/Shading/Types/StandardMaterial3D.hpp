#pragma once

#include <Shading/Material.hpp>
#include <Math/Colors.hpp>

namespace Ame::Rhi
{
    class StandardMaterial3D : public Material
    {
    public:
        struct Names
        {
            static constexpr const char BaseColorCst[]    = "BaseColor";
            static constexpr const char AmbientCst[]      = "Ambient";
            static constexpr const char SpecularCst[]     = "Specular";
            static constexpr const char EmissiveCst[]     = "Emissive";
            static constexpr const char TransparencyCst[] = "Transparent";

            static constexpr const char BaseColorMap[]    = "BaseColorMap";
            static constexpr const char NormalMap[]       = "NormalMap";
            static constexpr const char SpecularMap[]     = "SpecularMap";
            static constexpr const char RoughnessMap[]    = "RoughnessMap";
            static constexpr const char AmbientMap[]      = "AmbientMap";
            static constexpr const char EmissiveMap[]     = "EmissiveMap";
            static constexpr const char HeightMap[]       = "HeightMap";
            static constexpr const char ShininessMap[]    = "ShininessMap";
            static constexpr const char OpacityMap[]      = "OpacityMap";
            static constexpr const char DisplacementMap[] = "DisplacementMap";
            static constexpr const char LightMap[]        = "Lightmap";
            static constexpr const char MetallicMap[]     = "MetallicMap";
        };

    public:
        struct Constants
        {
            Math::Color4 BaseColor    = Colors::c_White;
            Math::Color4 Ambient      = Colors::c_Black;
            Math::Color4 Specular     = Colors::c_Black;
            Math::Color4 Emissive     = Colors::c_Black;
            float        Transparency = 1.0f;
        };

        struct CreateDesc : MaterialCreateDesc
        {
            CreateDesc();

            static constexpr std::array<Dg::PipelineResourceDesc, 12> StdResources{
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::BaseColorMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::NormalMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::SpecularMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::RoughnessMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::AmbientMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::EmissiveMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::HeightMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::ShininessMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::OpacityMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::DisplacementMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::LightMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_PIXEL, Names::MetallicMap, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
            };
        };
    };
} // namespace Ame::Rhi