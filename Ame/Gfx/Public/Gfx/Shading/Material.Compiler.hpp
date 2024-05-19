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
        [[nodiscard]] static Co::result<Ptr<Material>> Compile(
            Rhi::Device&              RhiDevice,
            MaterialPipelineState     PipelineState,
            const PropertyDescriptor& Descriptor);
    };
} // namespace Ame::Gfx::Shading