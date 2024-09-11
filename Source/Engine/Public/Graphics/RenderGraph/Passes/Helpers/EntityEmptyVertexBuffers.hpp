#pragma once

#include <Rg/Pass.hpp>
#include <Rhi/Utils/SRBBinder.hpp>
#include <Shading/VertexInput.hpp>

namespace Ame::Gfx
{
    class AME_ENGINE_API EntityEmptyVertexBuffersPass : public Rg::Pass
    {
    public:
        EntityEmptyVertexBuffersPass()
        {
            Build(std::bind_front(&EntityEmptyVertexBuffersPass::OnBuild, this));
        }

    private:
        void OnBuild(Rg::Resolver& resolver)
        {
            constexpr uint8_t bufferSize =
                Rhi::c_InputVertexAttributes[std::to_underlying(Rhi::VertexInputFlags::Count)].Offset;
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

            Ptr initData(Rhi::BufferInitData::Create(c_Data, sizeof(c_Data)));
            resolver.CreateBuffer(c_RGEntityEmptyVertexBuffers, initData, bufferDesc);
        }
    };
} // namespace Ame::Gfx