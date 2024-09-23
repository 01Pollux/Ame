#pragma once

#include <Rhi/Device/RhiDeviceCreateDesc.Export.h>

typedef struct Ame_IRhiDevice_t Ame_IRhiDevice_t;

// clang-format off
extern "C"
{
	// Must be released by Ame_IBaseObject_Release
	AME_ENGINE_C(Ame_IRhiDevice_t*, Ame_IRhiDevice_Create(const Ame_RhiDeviceCreateDesc_t* createDesc));

	AME_ENGINE_C(bool, Ame_IRhiDevice_BeginFrame(Ame_IRhiDevice_t* rhiDevice));
	AME_ENGINE_C(void, Ame_IRhiDevice_AdvanceFrame(Ame_IRhiDevice_t* rhiDevice, uint32_t syncInterval));

	AME_ENGINE_C(void*, Ame_IRhiDevice_GetFactory(Ame_IRhiDevice_t* rhiDevice));
	AME_ENGINE_C(void*, Ame_IRhiDevice_GetRenderDevice(Ame_IRhiDevice_t* rhiDevice));
	AME_ENGINE_C(void*, Ame_IRhiDevice_GetImmediateContext(Ame_IRhiDevice_t* rhiDevice));
	AME_ENGINE_C(Ame_IWindow_t*, Ame_IRhiDevice_GetWindow(Ame_IRhiDevice_t* rhiDevice));
	AME_ENGINE_C(void*, Ame_IRhiDevice_GetSwapchain(Ame_IRhiDevice_t* rhiDevice));
	AME_ENGINE_C(void*, Ame_IRhiDevice_GetCommonRenderPass(Ame_IRhiDevice_t* rhiDevice));
	AME_ENGINE_C(void*, Ame_IRhiDevice_GetRenderStateCache(Ame_IRhiDevice_t* rhiDevice));

	AME_ENGINE_C(DILIGENT_NAMESPACE(RENDER_DEVICE_TYPE), Ame_IRhiDevice_GetGraphicsAPI(Ame_IRhiDevice_t* rhiDevice));
	AME_ENGINE_C(const char*, Ame_IRhiDevice_GetGraphicsAPIName(Ame_IRhiDevice_t* rhiDevice, bool upperCase));
}
// clang-format on