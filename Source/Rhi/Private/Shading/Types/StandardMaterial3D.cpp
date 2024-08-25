#include <Shading/Types/StandardMaterial3D.hpp>
#include <Shaders/Rendering/Materials/StandardMaterial3D.hpp>

namespace Ame::Rhi
{
    StandardMaterial3D::CreateDesc::CreateDesc()
    {
        UserData
            .Float4(Names::BaseColorCst)
            .Float4(Names::AmbientCst)
            .Float4(Names::EmissiveCst)
            .Float4(Names::SpecularCst)
            .Float(Names::TransparencyCst);

        ResourceSignature.Resources    = StdResources.data();
        ResourceSignature.NumResources = Rhi::Count32(StdResources);

        Shaders.emplace(Dg::SHADER_TYPE_PIXEL, StdMaterial3D_PixelShader().GetCreateInfo());
    }
} // namespace Ame::Rhi