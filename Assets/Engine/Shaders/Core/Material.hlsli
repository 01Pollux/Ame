#ifndef AME_CORE_MATERIAL_H
#define AME_CORE_MATERIAL_H

#include "../Core/Structs.hlsli"
#include "../Core/FrameData.hlsli"
#include "../Core/RenderInstance.hlsli"
#include "../Geometry/Transform.hlsli"

MaterialFragment PSM_Main(Ecs_PSInput input);

#define MATERIALDATA_REGISTERSPACE 20
AME_ENABLE_DRAW_PARAMETERS;

#define AME_MATERIAL_RESOURCE(type, name, regType, bindingIndex) AME_RESOURCE(type, name, regType, bindingIndex, 20)

//

AME_RESOURCE(ConstantBuffer<FrameDesc>, g_FrameInfo, b, 0, 1);

AME_RESOURCE(StructuredBuffer<Transform>, g_Transforms, t, 0, 2);
AME_RESOURCE(StructuredBuffer<RenderInstance>, g_RenderInstances, t, 1, 2);

#endif