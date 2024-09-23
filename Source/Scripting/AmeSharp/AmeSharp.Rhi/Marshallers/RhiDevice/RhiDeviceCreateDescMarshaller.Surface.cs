using AmeSharp.Rhi.Surface;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Rhi.Marshallers.RhiDevice;

internal static unsafe partial class RhiDeviceCreateDescMarshaller
{
    internal struct UnmanagedRenderSurfaceDesc
    {
        public void* WindowNativePointer;
        public FullscreenModeDesc Fullscreen;
        public SwapchainDesc Swapchain;

        public static unsafe void* ToUnmanaged(RenderSurfaceDesc? desc)
        {
            if (desc is null)
            {
                return null;
            }

            var data = (UnmanagedRenderSurfaceDesc*)NativeMemory.Alloc((nuint)Unsafe.SizeOf<UnmanagedRenderSurfaceDesc>());
            data->WindowNativePointer = (void*)desc.Window.NativePointer;
            data->Fullscreen = desc.Fullscreen;
            data->Swapchain = desc.Swapchain;
            return data;
        }

        public static void Free(void* ptr)
        {
            if (ptr is not null)
            {
                NativeMemory.Free(ptr);
            }
        }
    }
}
