using AmeSharp.Rhi.RhiDevice.Desc;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Rhi.Marshallers.RhiDevice;

internal static unsafe partial class RhiDeviceCreateDescMarshaller
{
    internal enum UnmanagedDescType : int
    {
        None,
        D3D11,
        D3D12,
        Gl,
        Vulkan
    }

    internal struct UnmanagedDescStreamType
    {
        public UnmanagedDescType Type;
        public void* Data;

        private void FromManaged(IRhiDeviceTypeDesc type, StringPool pool)
        {
            switch (type)
            {
            case D3D11RhiDeviceTypeDesc d3d11:
            {
                Type = UnmanagedDescType.D3D11;
                var data = (UnmanagedD3D11RhiDeviceTypeDesc*)(Data = NativeMemory.Alloc((nuint)Unsafe.SizeOf<UnmanagedD3D11RhiDeviceTypeDesc>()));
                data->Copy(d3d11);
                break;
            }
            case D3D12RhiDeviceTypeDesc d3d12:
            {
                Type = UnmanagedDescType.D3D12;
                var data = (UnmanagedD3D12RhiDeviceTypeDesc*)(Data = NativeMemory.Alloc((nuint)Unsafe.SizeOf<UnmanagedD3D12RhiDeviceTypeDesc>()));
                data->Copy(d3d12);
                break;
            }
            case GLRhiDeviceTypeDesc _:
            {
                Type = UnmanagedDescType.Gl;
                Data = null;
                break;
            }
            case VulkanRhiDeviceTypeDesc vulkan:
            {
                Type = UnmanagedDescType.Vulkan;
                var data = (UnmanagedVulkanRhiDeviceTypeDesc*)(Data = NativeMemory.Alloc((nuint)Unsafe.SizeOf<UnmanagedVulkanRhiDeviceTypeDesc>()));
                data->Copy(vulkan, pool);
                break;
            }
            }
        }

        public static void* ToUnmanaged(IRhiDeviceTypeDesc[] types, StringPool pool)
        {
            var byteSize = Unsafe.SizeOf<UnmanagedDescStreamType>() * (types.Length + 1);
            if (byteSize == 0)
            {
                return null;
            }

            var data = NativeMemory.Alloc((nuint)byteSize);
            for (int i = 0; i < types.Length; i++)
            {
                unsafe
                {
                    UnmanagedDescStreamType* target = ((UnmanagedDescStreamType*)data) + i;
                    target->FromManaged(types[i], pool);
                }
            }
            unsafe
            {
                UnmanagedDescStreamType* target = ((UnmanagedDescStreamType*)data) + types.Length;
                target->Type = UnmanagedDescType.None;
                target->Data = null;
            }

            return data;
        }

        public static void Free(void* data)
        {
            if (data is null)
            {
                return;
            }
            var type = (UnmanagedDescStreamType*)data;
            while (type->Type != UnmanagedDescType.None)
            {
                NativeMemory.Free(type->Data);
                type++;
            }
            NativeMemory.Free(data);
        }
    }
}
