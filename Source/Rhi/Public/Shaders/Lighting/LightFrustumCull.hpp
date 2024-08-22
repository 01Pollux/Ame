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
    class LightFrustumCull_ComputeShader : public EngineShader
    {
    private:
        static constexpr const char* DISPATCH_CHUNK_SIZE   = "DISPATCH_CHUNK_SIZE";
        static constexpr const char* MAX_LIGHT_CHUNK_SIZE  = "MAX_LIGHT_CHUNK_SIZE";
        static constexpr const char  c_DispatchChunkSize[] = "16";
        static constexpr const char  c_MaxLightChunkSize[] = "1024";

        static constexpr const char c_SourceCode[] =
#ifndef AME_DIST
#include "LightFrustumCull_CS_Debug.hlsli.generated.hpp"
#include "LightFrustumCull_CS.hlsli.generated.hpp"
#else
#include "Shaders/LightFrustumCull_CS_DebugNone.hlsli.generated.hpp"
#include "Shaders/LightFrustumCull_CS.hlsli.generated.hpp"
#endif
            ;

    public:
        LightFrustumCull_ComputeShader(
            const char* dispatchChunkSize = c_DispatchChunkSize,
            const char* maxLightChunkSize = c_MaxLightChunkSize) :
            m_Macros{
                Dg::ShaderMacro{ DISPATCH_CHUNK_SIZE, dispatchChunkSize },
                Dg::ShaderMacro{ MAX_LIGHT_CHUNK_SIZE, maxLightChunkSize }
            }
        {
            Setup({ "L_LightFrustumCull_CS", Dg::SHADER_TYPE_COMPUTE }, c_SourceCode);
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

        std::array<Dg::ShaderMacro, 2> m_Macros;
    };
} // namespace Ame::Rhi