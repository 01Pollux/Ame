#pragma once

#include <Core/Coroutine.hpp>
#include <Gfx/Shading/PropertyDescriptor.hpp>

namespace Ame::Gfx::Shading
{
    class MaterialCompiler
    {
    public:
        /// <summary>
        /// Compiles the material with the given parameters and descriptors
        /// </summary>
        static Co::result<Ptr<Material>> Compile(
            Rhi::Device&                 RhiDevice,
            const MaterialPipelineState& PipelineDesc,
            const PropertyDescriptor&    Descriptor);

    private:
        static Co::result<Ptr<Material>> Compile(
            const Ptr<Rhi::PipelineState>& PipelineState,
            const MaterialPipelineState&   PipelineDesc,
            const PropertyDescriptor&      Descriptor);
    };
} // namespace Ame::Gfx::Shading