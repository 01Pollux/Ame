#pragma once

#include <Core/Hash.hpp>

#include <Rhi/Descs/Pipeline.hpp>
#include <Rhi/Resource/PipelineLayout.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::ShaderDesc>
    {
        size_t operator()(
            const Ame::Rhi::ShaderDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, std::to_underlying(Desc.stage));
            Ame::HashCombine(Hash, std::string_view{ static_cast<const char*>(Desc.bytecode), Desc.size });

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::InputAssemblyDesc>
    {
        size_t operator()(
            const Ame::Rhi::InputAssemblyDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, std::to_underlying(Desc.Topology));
            Ame::HashCombine(Hash, Desc.TessControlPointNum);
            Ame::HashCombine(Hash, std::to_underlying(Desc.PrimitiveRestart));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::RasterizationDesc>
    {
        size_t operator()(
            const Ame::Rhi::RasterizationDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.DepthBias);
            Ame::HashCombine(Hash, Desc.DepthBiasClamp);
            Ame::HashCombine(Hash, Desc.DepthBiasSlopeFactor);
            Ame::HashCombine(Hash, Desc.ViewportNum);
            Ame::HashCombine(Hash, Desc.DepthClamp);
            Ame::HashCombine(Hash, std::to_underlying(Desc.Fill));
            Ame::HashCombine(Hash, std::to_underlying(Desc.Cull));
            Ame::HashCombine(Hash, Desc.FrontCounterClockwise);
            Ame::HashCombine(Hash, Desc.AntialiasedLines);
            Ame::HashCombine(Hash, Desc.ConservativeRasterization);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::BlendingDesc>
    {
        size_t operator()(
            const Ame::Rhi::BlendingDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, std::to_underlying(Desc.Src));
            Ame::HashCombine(Hash, std::to_underlying(Desc.Dst));
            Ame::HashCombine(Hash, std::to_underlying(Desc.Func));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::RenderTargetDesc>
    {
        size_t operator()(
            const Ame::Rhi::RenderTargetDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, std::to_underlying(Desc.Format));
            Ame::HashCombine(Hash, Desc.Color);
            Ame::HashCombine(Hash, Desc.Alpha);
            Ame::HashCombine(Hash, std::to_underlying(Desc.WriteMask));
            Ame::HashCombine(Hash, Desc.BlendEnable);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::DepthTargetDesc>
    {
        size_t operator()(
            const Ame::Rhi::DepthTargetDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, std::to_underlying(Desc.Func));
            Ame::HashCombine(Hash, Desc.WriteEnable);
            Ame::HashCombine(Hash, Desc.BoundsTestEnable);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::StencilDesc>
    {
        size_t operator()(
            const Ame::Rhi::StencilDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.WriteMask);
            Ame::HashCombine(Hash, Desc.CompareMask);
            Ame::HashCombine(Hash, std::to_underlying(Desc.Func));
            Ame::HashCombine(Hash, std::to_underlying(Desc.OnFail));
            Ame::HashCombine(Hash, std::to_underlying(Desc.OnDepthFail));
            Ame::HashCombine(Hash, std::to_underlying(Desc.OnPass));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::StencilTargetDesc>
    {
        size_t operator()(
            const Ame::Rhi::StencilTargetDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.Front);
            Ame::HashCombine(Hash, Desc.Back);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::OutputMergerDesc>
    {
        size_t operator()(
            const Ame::Rhi::OutputMergerDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            for (auto& RT : Desc.RenderTargets)
            {
                Ame::HashCombine(Hash, RT);
            }
            Ame::HashCombine(Hash, std::to_underlying(Desc.DepthStencilFormat));
            Ame::HashCombine(Hash, Desc.DepthTarget);
            Ame::HashCombine(Hash, Desc.StencilTarget);
            Ame::HashCombine(Hash, std::to_underlying(Desc.ColorLogicFunc));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::VertexAttributeDesc>
    {
        size_t operator()(
            const Ame::Rhi::VertexAttributeDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.d3d.semanticIndex);
            Ame::HashCombine(Hash, Desc.d3d.semanticName);
            Ame::HashCombine(Hash, Desc.vk.location);
            Ame::HashCombine(Hash, Desc.offset);
            Ame::HashCombine(Hash, std::to_underlying(Desc.format));
            Ame::HashCombine(Hash, Desc.streamIndex);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::VertexStreamDesc>
    {
        size_t operator()(
            const Ame::Rhi::VertexStreamDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.stride);
            Ame::HashCombine(Hash, Desc.bindingSlot);
            Ame::HashCombine(Hash, std::to_underlying(Desc.stepRate));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::VertexInputDesc>
    {
        size_t operator()(
            const Ame::Rhi::VertexInputDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            for (uint8_t i = 0; i < Desc.attributeNum; i++)
            {
                Ame::HashCombine(Hash, Desc.attributes[i]);
            }
            for (uint8_t i = 0; i < Desc.streamNum; i++)
            {
                Ame::HashCombine(Hash, Desc.streams[i]);
            }

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::MultisampleDesc>
    {
        size_t operator()(
            const Ame::Rhi::MultisampleDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.SampleMask);
            Ame::HashCombine(Hash, Desc.SampleCount);
            Ame::HashCombine(Hash, Desc.AlphaToCoverageEnable);
            Ame::HashCombine(Hash, Desc.ProgrammableSampleLocations);

            return Hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::GraphicsPipelineDesc>
    {
        size_t operator()(
            const Ame::Rhi::GraphicsPipelineDesc& Desc) const noexcept
        {
            size_t Hash = Desc.Layout->GetHash();

            Ame::HashCombine(Hash, Desc.InputAssembly);
            Ame::HashCombine(Hash, Desc.Rasterizer);
            Ame::HashCombine(Hash, Desc.OutputMerger);
            for (auto& Shader : Desc.Shaders)
            {
                Ame::HashCombine(Hash, Shader);
            }
            if (Desc.VertexInput)
            {
                Ame::HashCombine(Hash, *Desc.VertexInput);
            }
            if (Desc.Multisample)
            {
                Ame::HashCombine(Hash, *Desc.Multisample);
            }

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::ComputePipelineDesc>
    {
        size_t operator()(
            const Ame::Rhi::ComputePipelineDesc& Desc) const noexcept
        {
            size_t Hash = Desc.Layout->GetHash();

            Ame::HashCombine(Hash, Desc.Shader);

            return Hash;
        }
    };
} // namespace std
