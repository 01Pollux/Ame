#pragma once

#include <Core/Coroutine.hpp>
#include <Gfx/Shading/PropertyDescriptor.hpp>

namespace Ame::Gfx::Cache
{
    class ShaderCache;
} // namespace Ame::Gfx::Cache

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
            Cache::ShaderCache&       ShaderCache,
            MaterialPipelineState     PipelineState,
            const PropertyDescriptor& Descriptor);
    };
} // namespace Ame::Gfx::Shading