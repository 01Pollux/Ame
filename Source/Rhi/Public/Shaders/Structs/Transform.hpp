#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructTransformShader : public EngineShader
    {
    private:
        // clang-format off
        static constexpr const char c_SourceCode[] = R"(
#ifndef STRUCT_TRANSFORM
#define STRUCT_TRANSFORM
struct Transform{float4x4 World;};
float3 transform_get_position(const in Transform transform){return transform.World[3].xyz;}
float3 transform_get_forward(const in Transform transform){return transform.World[2].xyz;}
float3 transform_get_up(const in Transform transform){return transform.World[1].xyz;}
float3 transform_get_right(const in Transform transform){return transform.World[0].xyz;}
#endif
)";
        // clang-format on

    public:
        static constexpr const char* Name = "Structs/Transform.hlsli";

        StructTransformShader()
        {
            Setup({ "S_Transform", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi