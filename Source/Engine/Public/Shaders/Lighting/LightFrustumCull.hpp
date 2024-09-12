#pragma once

#include <Shaders/EngineShader.hpp>

#include <Shaders/Structs/Transform.hpp>
#include <Shaders/Structs/Light.hpp>
#include <Shaders/Structs/RenderInstance.hpp>
#include <Shaders/Structs/CameraFrameData.hpp>

#include <Shaders/Structs/Geometry/Common.hpp>
#include <Shaders/Structs/Geometry/Plane.hpp>
#include <Shaders/Structs/Geometry/Frustum.hpp>
#include <Shaders/Structs/Geometry/Sphere.hpp>

#include <Shaders/Utils/ViewConverter.hpp>
#include <Shaders/Structs/Geometry/Operations.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API LightFrustumCull_ComputeShader : public EngineShader
    {
    private:
        static constexpr const char* DISPATCH_CHUNK_SIZE  = "DISPATCH_CHUNK_SIZE";
        static constexpr const char* MAX_LIGHT_CHUNK_SIZE = "MAX_LIGHT_CHUNK_SIZE";

        static constexpr const char c_SourceCode[] =
#ifndef AME_DIST
#include "LightFrustumCull_CS_Debug.hlsli.generated.hpp"
#include "LightFrustumCull_CS.hlsli.generated.hpp"
#else
#include "LightFrustumCull_CS_DebugNone.hlsli.generated.hpp"
#include "LightFrustumCull_CS.hlsli.generated.hpp"
#endif
            ;

    public:
        LightFrustumCull_ComputeShader(uint8_t blockSize, uint16_t maxLightChunkSize) :
            m_DispatchChunkSize(std::to_string(blockSize)), m_MaxLightChunkSize(std::to_string(maxLightChunkSize)),
            m_Macros{ Dg::ShaderMacro{ DISPATCH_CHUNK_SIZE, m_DispatchChunkSize.c_str() },
                      Dg::ShaderMacro{ MAX_LIGHT_CHUNK_SIZE, m_MaxLightChunkSize.c_str() } }
        {
            Setup({ "L_LightFrustumCull_CS", Dg::SHADER_TYPE_COMPUTE, true }, c_SourceCode);
            m_CreateInfo.Macros = { m_Macros.data(), Count32(m_Macros) };
            LinkShaders();
        }

    private:
        void LinkShaders()
        {
            StructTransformShader       transformStruct;
            StructLightShader           lightStruct;
            StructRenderInstanceShader  renderInstanceStruct;
            StructCameraFrameDataShader cameraFrameDataStruct;

            GeometryCommonShader              geometryCommonShader;
            StructGeometryPlanesShader        geometryPlanesStruct;
            StructGeometryFrustumPlanesShader geometryFrustumPlanesStruct;
            StructGeometrySphereShader        geometrySphereStruct;

            UtilsViewConverterShader viewConverterStruct;
            GeometryOperationsShader geometryOperationsStruct;

            m_LinkedShaders = Dg::CreateMemoryShaderSourceFactory(
                {
                    { transformStruct.GetMemoryShaderSourceFileInfo(transformStruct.Name) },
                    { lightStruct.GetMemoryShaderSourceFileInfo(lightStruct.Name) },
                    { renderInstanceStruct.GetMemoryShaderSourceFileInfo(renderInstanceStruct.Name) },
                    { cameraFrameDataStruct.GetMemoryShaderSourceFileInfo(cameraFrameDataStruct.Name) },

                    { geometryCommonShader.GetMemoryShaderSourceFileInfo(geometryCommonShader.Name) },
                    { geometryPlanesStruct.GetMemoryShaderSourceFileInfo(geometryPlanesStruct.Name) },
                    { geometryFrustumPlanesStruct.GetMemoryShaderSourceFileInfo(geometryFrustumPlanesStruct.Name) },
                    { geometrySphereStruct.GetMemoryShaderSourceFileInfo(geometrySphereStruct.Name) },

                    { viewConverterStruct.GetMemoryShaderSourceFileInfo(viewConverterStruct.Name) },
                    { geometryOperationsStruct.GetMemoryShaderSourceFileInfo(geometryOperationsStruct.Name) },
                },
                true);
            m_CreateInfo.pShaderSourceStreamFactory = m_LinkedShaders;
        }

    private:
        Ptr<Dg::IShaderSourceInputStreamFactory> m_LinkedShaders;

        const String m_DispatchChunkSize;
        const String m_MaxLightChunkSize;

        std::array<Dg::ShaderMacro, 2> m_Macros;
    };
} // namespace Ame::Rhi