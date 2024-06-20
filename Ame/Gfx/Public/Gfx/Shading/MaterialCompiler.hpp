#pragma once

#include <Core/Coroutine.hpp>
#include <Gfx/Shading/PropertyDescriptor.hpp>

namespace Ame
{
    namespace Rhi
    {
        class DeviceResourceAllocator;
    } // namespace Rhi
    namespace Gfx::Cache
    {
        class CommonPipelineLayout;
        class ShaderCache;
        class CommonPipelineState;
    } // namespace Gfx::Cache
} // namespace Ame

namespace Ame::Gfx::Shading
{
    class MaterialCompiler
    {
    public:
        /// <summary>
        /// Compiles the material with the given parameters and descriptors
        /// </summary>
        [[nodiscard]] static Co::result<Ptr<Material>> Compile(
            Rhi::DeviceResourceAllocator& allocator,
            Cache::CommonPipelineLayout&  pipelineLayoutCache,
            Cache::ShaderCache&           shaderCache,
            Cache::CommonPipelineState&   pipelineStateCache,
            MaterialPipelineState         pipelineState,
            const PropertyDescriptor&     descriptor);

    public:
        MaterialCompiler(
            Rhi::Device&                 rhiDevice,
            Cache::CommonPipelineLayout& pipelineLayoutCache,
            Cache::ShaderCache&          shaderCache,
            Cache::CommonPipelineState&  pipelineStateCache);

    public:
        /// <summary>
        /// Compiles the material with the given parameters and descriptors
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Material>> Compile(
            MaterialPipelineState     pipelineState,
            const PropertyDescriptor& descriptor)
        {
            return Compile(m_Allocator, m_PipelineLayoutCache, m_ShaderCache, m_PipelineStateCache, std::move(pipelineState), descriptor);
        }

    private:
        Ref<Rhi::DeviceResourceAllocator> m_Allocator;

        Ref<Cache::CommonPipelineLayout> m_PipelineLayoutCache;
        Ref<Cache::ShaderCache>          m_ShaderCache;
        Ref<Cache::CommonPipelineState>  m_PipelineStateCache;
    };
} // namespace Ame::Gfx::Shading