#pragma once

#include <Core/hash.hpp>

#include <Rhi/Descs/Pipeline.hpp>
#include <Rhi/Resource/PipelineLayout.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::ShaderDesc>
    {
        size_t operator()(
            const Ame::Rhi::ShaderDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, std::to_underlying(desc.stage));
            Ame::HashCombine(hash, std::string_view{ static_cast<const char*>(desc.bytecode), desc.size });

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::InputAssemblyDesc>
    {
        size_t operator()(
            const Ame::Rhi::InputAssemblyDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, std::to_underlying(desc.Topology));
            Ame::HashCombine(hash, desc.TessControlPointNum);
            Ame::HashCombine(hash, std::to_underlying(desc.PrimitiveRestart));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::RasterizationDesc>
    {
        size_t operator()(
            const Ame::Rhi::RasterizationDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.DepthBias);
            Ame::HashCombine(hash, desc.DepthBiasClamp);
            Ame::HashCombine(hash, desc.DepthBiasSlopeFactor);
            Ame::HashCombine(hash, desc.ViewportNum);
            Ame::HashCombine(hash, desc.DepthClamp);
            Ame::HashCombine(hash, std::to_underlying(desc.Fill));
            Ame::HashCombine(hash, std::to_underlying(desc.Cull));
            Ame::HashCombine(hash, desc.FrontCounterClockwise);
            Ame::HashCombine(hash, desc.AntialiasedLines);
            Ame::HashCombine(hash, desc.ConservativeRasterization);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::BlendingDesc>
    {
        size_t operator()(
            const Ame::Rhi::BlendingDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, std::to_underlying(desc.Src));
            Ame::HashCombine(hash, std::to_underlying(desc.Dst));
            Ame::HashCombine(hash, std::to_underlying(desc.Func));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::RenderTargetDesc>
    {
        size_t operator()(
            const Ame::Rhi::RenderTargetDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, std::to_underlying(desc.Format));
            Ame::HashCombine(hash, desc.Color);
            Ame::HashCombine(hash, desc.Alpha);
            Ame::HashCombine(hash, std::to_underlying(desc.WriteMask));
            Ame::HashCombine(hash, desc.BlendEnable);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::DepthTargetDesc>
    {
        size_t operator()(
            const Ame::Rhi::DepthTargetDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, std::to_underlying(desc.Func));
            Ame::HashCombine(hash, desc.WriteEnable);
            Ame::HashCombine(hash, desc.BoundsTestEnable);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::StencilDesc>
    {
        size_t operator()(
            const Ame::Rhi::StencilDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.WriteMask);
            Ame::HashCombine(hash, desc.CompareMask);
            Ame::HashCombine(hash, std::to_underlying(desc.Func));
            Ame::HashCombine(hash, std::to_underlying(desc.OnFail));
            Ame::HashCombine(hash, std::to_underlying(desc.OnDepthFail));
            Ame::HashCombine(hash, std::to_underlying(desc.OnPass));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::StencilTargetDesc>
    {
        size_t operator()(
            const Ame::Rhi::StencilTargetDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.Front);
            Ame::HashCombine(hash, desc.Back);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::OutputMergerDesc>
    {
        size_t operator()(
            const Ame::Rhi::OutputMergerDesc& desc) const noexcept
        {
            size_t hash = 0;

            for (auto& rt : desc.RenderTargets)
            {
                Ame::HashCombine(hash, rt);
            }
            Ame::HashCombine(hash, std::to_underlying(desc.DepthStencilFormat));
            Ame::HashCombine(hash, desc.DepthTarget);
            Ame::HashCombine(hash, desc.StencilTarget);
            Ame::HashCombine(hash, std::to_underlying(desc.ColorLogicFunc));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::VertexAttributeDesc>
    {
        size_t operator()(
            const Ame::Rhi::VertexAttributeDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.d3d.semanticIndex);
            Ame::HashCombine(hash, desc.d3d.semanticName);
            Ame::HashCombine(hash, desc.vk.location);
            Ame::HashCombine(hash, desc.offset);
            Ame::HashCombine(hash, std::to_underlying(desc.format));
            Ame::HashCombine(hash, desc.streamIndex);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::VertexStreamDesc>
    {
        size_t operator()(
            const Ame::Rhi::VertexStreamDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.stride);
            Ame::HashCombine(hash, desc.bindingSlot);
            Ame::HashCombine(hash, std::to_underlying(desc.stepRate));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::VertexInputDesc>
    {
        size_t operator()(
            const Ame::Rhi::VertexInputDesc& desc) const noexcept
        {
            size_t hash = 0;

            for (uint8_t i = 0; i < desc.attributeNum; i++)
            {
                Ame::HashCombine(hash, desc.attributes[i]);
            }
            for (uint8_t i = 0; i < desc.streamNum; i++)
            {
                Ame::HashCombine(hash, desc.streams[i]);
            }

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::MultisampleDesc>
    {
        size_t operator()(
            const Ame::Rhi::MultisampleDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.SampleMask);
            Ame::HashCombine(hash, desc.SampleCount);
            Ame::HashCombine(hash, desc.AlphaToCoverageEnable);
            Ame::HashCombine(hash, desc.ProgrammableSampleLocations);

            return hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::GraphicsPipelineDesc>
    {
        size_t operator()(
            const Ame::Rhi::GraphicsPipelineDesc& desc) const noexcept
        {
            size_t hash = desc.Layout->GetHash();

            Ame::HashCombine(hash, desc.InputAssembly);
            Ame::HashCombine(hash, desc.Rasterizer);
            Ame::HashCombine(hash, desc.OutputMerger);
            for (auto& Shader : desc.Shaders)
            {
                Ame::HashCombine(hash, Shader);
            }
            if (desc.VertexInput)
            {
                Ame::HashCombine(hash, *desc.VertexInput);
            }
            if (desc.Multisample)
            {
                Ame::HashCombine(hash, *desc.Multisample);
            }

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::ComputePipelineDesc>
    {
        size_t operator()(
            const Ame::Rhi::ComputePipelineDesc& desc) const noexcept
        {
            size_t hash = desc.Layout->GetHash();

            Ame::HashCombine(hash, desc.Shader);

            return hash;
        }
    };
} // namespace std
