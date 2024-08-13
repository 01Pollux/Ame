#include <Shading/Types/StandardMaterial3D.hpp>

namespace Ame::Rhi
{
    StandardMaterial3D::CreateDesc::CreateDesc()
    {
        UserData
            .Float4(Names::BaseColorCst)
            .Float4(Names::AmbientCst)
            .Float4(Names::SpecularCst)
            .Float4(Names::EmissiveCst)
            .Float(Names::TransparencyCst);

        ResourceSignature.Resources    = StdResources.data();
        ResourceSignature.NumResources = Rhi::Count32(StdResources);
    }
} // namespace Ame::Rhi