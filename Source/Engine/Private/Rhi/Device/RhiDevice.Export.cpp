#include <Rhi/Device/RhiDeviceImpl.hpp>
#include <Rhi/Device/RhiDevice.Export.h>
#include <Rhi/Device/RhiDeviceCreateDesc.hpp>

using namespace Ame::Rhi;

using StringPoolList = std::vector<std::vector<const char*>>;

static DeviceCreateDesc GetCreateDesc(const Ame_RhiDeviceCreateDesc_t* desc, StringPoolList& stringPools)
{
    DeviceCreateDesc createDesc{ .Features        = desc->Features,
                                 .ValidationLayer = static_cast<DeviceValidationType>(desc->ValidationLayer) };

    auto typeIter = desc->Types;
    while (typeIter->Type != AME_RHI_DEVICE_CREATE_NONE)
    {
        auto& curDesc = typeIter->Desc;
        switch (typeIter->Type)
        {
        case AME_RHI_DEVICE_CREATE_D3D11:
        {
            auto d3d11Desc = std::bit_cast<Ame_RhiDeviceCreateDescD3D11_t*>(curDesc);
            createDesc.Types.emplace_back(DeviceCreateDescD3D11{ .GraphicsAPIVersion = d3d11Desc->GraphicsAPIVersion });
            break;
        }
        case AME_RHI_DEVICE_CREATE_D3D12:
        {
            auto  d3d12Desc  = std::bit_cast<Ame_RhiDeviceCreateDescD3D12_t*>(curDesc);
            auto& targetDesc = std::get<DeviceCreateDescD3D12>(createDesc.Types.emplace_back(DeviceCreateDescD3D12{}));
            std::memcpy(d3d12Desc, &targetDesc, sizeof(targetDesc));
            break;
        }
        case AME_RHI_DEVICE_CREATE_GL:
        {
            createDesc.Types.emplace_back(DeviceCreateDescGL());
            break;
        }
        case AME_RHI_DEVICE_CREATE_VULKAN:
        {
            auto  vkDesc = std::bit_cast<Ame_RhiDeviceCreateDescVulkan_t*>(curDesc);
            auto& targetDesc =
                std::get<DeviceCreateDescVulkan>(createDesc.Types.emplace_back(DeviceCreateDescVulkan{}));

            std::memcpy(
                &vkDesc->MainDescriptorPool, &targetDesc.MainDescriptorPool, sizeof(vkDesc->MainDescriptorPool));
            std::memcpy(&vkDesc->DynamicDescriptorPool, &targetDesc.DynamicDescriptorPool,
                        sizeof(vkDesc->DynamicDescriptorPool));

            auto insertToPool = [&stringPools](const char** list) -> std::span<const char*>
            {
                if (list)
                {
                    auto& pool = stringPools.emplace_back();
                    while (*list != nullptr)
                    {
                        pool.emplace_back(*list);
                    }
                    return pool;
                }
                return {};
            };

            targetDesc.RequiredLayerExtensions    = insertToPool(vkDesc->RequiredLayerExtensions);
            targetDesc.RequiredInstanceExtensions = insertToPool(vkDesc->RequiredInstanceExtensions);
            targetDesc.RequiredDeviceExtensions   = insertToPool(vkDesc->RequiredDeviceExtensions);
            targetDesc.IgnoredDebugMessageNames   = insertToPool(vkDesc->IgnoredDebugMessageNames);

            targetDesc.RequiredDeviceFeatures       = vkDesc->RequiredDeviceFeatures;
            targetDesc.DeviceLocalMemoryPageSize    = vkDesc->DeviceLocalMemoryPageSize;
            targetDesc.HostVisibleMemoryPageSize    = vkDesc->HostVisibleMemoryPageSize;
            targetDesc.DeviceLocalMemoryReserveSize = vkDesc->DeviceLocalMemoryReserveSize;
            targetDesc.HostVisibleMemoryReserveSize = vkDesc->HostVisibleMemoryReserveSize;
            targetDesc.UploadHeapPageSize           = vkDesc->UploadHeapPageSize;
            targetDesc.DynamicHeapSize              = vkDesc->DynamicHeapSize;
            targetDesc.DynamicHeapPageSize          = vkDesc->DynamicHeapPageSize;

            std::memcpy(&vkDesc->QueryHeapSizes, &targetDesc.QueryHeapSizes, sizeof(vkDesc->QueryHeapSizes));
            break;
        }
        }
        typeIter++;
    }

    if (desc->AdapterCallback)
    {
        createDesc.AdapterCallback = [desc](std::span<const Dg::GraphicsAdapterInfo> adapters) -> Ame::Opt<uint32_t>
        {
            auto result = desc->AdapterCallback(adapters.data(), static_cast<uint32_t>(adapters.size()));
            return result != static_cast<uint32_t>(-1) ? Ame::Opt<uint32_t>{ result } : std::nullopt;
        };
    }

    if (desc->Surface)
    {
        auto& surfaceDesc = createDesc.Surface.emplace();

        surfaceDesc.Window = std::bit_cast<Ame::Window::IWindow*>(desc->Surface->Window);

        auto& fullScreenDesc       = desc->Surface->FullscreenMode;
        surfaceDesc.FullscreenMode = { .RefreshRate   = { fullScreenDesc.RefreshRate.Numerator,
                                                          fullScreenDesc.RefreshRate.Denominator },
                                       .Scaling       = fullScreenDesc.Scaling,
                                       .ScanlineOrder = fullScreenDesc.ScanlineOrder,
                                       .Fullscreen    = fullScreenDesc.Fullscreen };

        auto& swapchainDesc   = desc->Surface->Swapchain;
        surfaceDesc.Swapchain = {
            .ColorFormat         = swapchainDesc.ColorFormat,
            .DepthFormat         = swapchainDesc.DepthFormat,
            .Usage               = swapchainDesc.Usage,
            .PreTransform        = swapchainDesc.PreTransform,
            .BufferCount         = swapchainDesc.BufferCount,
            .DefaultDepthValue   = swapchainDesc.DefaultDepthValue,
            .DefaultStencilValue = swapchainDesc.DefaultStencilValue,
            .IsPrimary           = swapchainDesc.IsPrimary,
        };
    }

    return createDesc;
}

Ame_IRhiDevice_t* Ame_IRhiDevice_Create(const Ame_RhiDeviceCreateDesc_t* createDesc)
{
    StringPoolList stringPools;
    return std::bit_cast<Ame_IRhiDevice_t*>(AmeCreateRaw(RhiDeviceImpl, GetCreateDesc(createDesc, stringPools)));
}

bool Ame_IRhiDevice_BeginFrame(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->BeginFrame();
}

void* Ame_IRhiDevice_GetFactory(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->GetFactory();
}

void* Ame_IRhiDevice_GetRenderDevice(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->GetRenderDevice();
}

void* Ame_IRhiDevice_GetImmediateContext(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->GetImmediateContext();
}

Ame_IWindow_t* Ame_IRhiDevice_GetWindow(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return std::bit_cast<Ame_IWindow_t*>(impl->GetWindow());
}

void* Ame_IRhiDevice_GetSwapchain(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->GetSwapchain();
}

void* Ame_IRhiDevice_GetCommonRenderPass(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->GetCommonRenderPass();
}

void* Ame_IRhiDevice_GetRenderStateCache(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->GetRenderStateCache();
}

DILIGENT_NAMESPACE(RENDER_DEVICE_TYPE) Ame_IRhiDevice_GetGraphicsAPI(Ame_IRhiDevice_t* rhiDevice)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->GetGraphicsAPI();
}

const char* Ame_IRhiDevice_GetGraphicsAPIName(Ame_IRhiDevice_t* rhiDevice, bool upperCase)
{
    auto impl = std::bit_cast<IRhiDevice*>(rhiDevice);
    return impl->GetGraphicsAPIName(upperCase);
}
