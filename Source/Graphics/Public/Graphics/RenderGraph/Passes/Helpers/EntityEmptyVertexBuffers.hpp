#pragma once

#include <RG/Pass.hpp>
#include <Rhi/Utils/SRBBinder.hpp>
#include <Shading/VertexInput.hpp>

namespace Ame::Gfx
{
    class EntityEmptyVertexBuffersPass : public RG::Pass
    {
    public:
        static inline const RG::ResourceId EGEntityEmptyVertexBuffers{ "_EEVB" };

    public:
        EntityEmptyVertexBuffersPass()
        {
            Build(std::bind_front(&EntityEmptyVertexBuffersPass::OnBuild, this));
        }

    private:
        void OnBuild(
            RG::Resolver& resolver)
        {
            constexpr uint8_t        bufferSize = Rhi::c_InputVertexAttributes[std::to_underlying(Rhi::VertexInputFlags::Count)].Offset;
            constexpr Dg::BufferDesc bufferDesc{
#ifndef AME_DIST
                "_EEVB",
#else
                nullptr,
#endif
                bufferSize,
                Dg::BIND_VERTEX_BUFFER,
                Dg::USAGE_IMMUTABLE,
            };

            // clang-format off
            constexpr float c_Data[bufferSize / sizeof(float)] = { 
                0.f, 0.f, 0.f, // Position
                1.f, 0.f, 0.f, // Normal
                0.f, 0.f,      // TexCoord
                1.f, 0.f, 0.f  // Tangent
            };
            // clang-format on

            Dg::BufferData bufferData(c_Data, sizeof(c_Data));
            resolver.CreateImmediateBuffer(EGEntityEmptyVertexBuffers, &bufferData, bufferDesc);
        }
    };
} // namespace Ame::Gfx