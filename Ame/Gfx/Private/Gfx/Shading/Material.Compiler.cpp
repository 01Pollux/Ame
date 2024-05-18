#include <Gfx/Shading/Material.Compiler.hpp>

#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx::Shading
{
    Co::result<Ptr<Material>> MaterialCompiler::Compile(
        Rhi::Device&                 RhiDevice,
        const MaterialPipelineState& PipelineDesc,
        const PropertyDescriptor&    Descriptor)
    {
        co_return nullptr;
    }

    //

    Co::result<Ptr<Material>> MaterialCompiler::Compile(
        const Ptr<Rhi::PipelineState>& PipelineState,
        const MaterialPipelineState&   PipelineDesc,
        const PropertyDescriptor&      Descriptor)
    {
        return Co::result<Ptr<Material>>();
    }
} // namespace Ame::Gfx::Shading