using AmeSharp.Rhi.RhiDevice.Desc;
using Diligent;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Rhi.Marshallers.RhiDevice;

[CustomMarshaller(typeof(RhiDeviceCreateDesc), MarshalMode.ManagedToUnmanagedIn, typeof(RhiDeviceCreateDescMarshaller))]
internal static unsafe partial class RhiDeviceCreateDescMarshaller
{
    internal class UserDataState(RhiDeviceCreateDesc managed)
    {
        public RhiDeviceCreateDesc Desc { get; private set; } = managed;
        public StringPool Pool { get; private set; } = new();
    }
    internal struct Unmanaged
    {
        public DeviceFeatures Features;
        public delegate* unmanaged[Cdecl]<Unmanaged*, UnmanagedGraphicsAdapterInfo*, uint, uint> AdapterCallback;
        public uint ValidationLayer;
        public void* Types;
        public void* Surface;
        public IntPtr UserData;
    }

    public static Unmanaged ConvertToUnmanaged(RhiDeviceCreateDesc managed)
    {
        UserDataState userData = new(managed);
        var handle = GCHandle.ToIntPtr(GCHandle.Alloc(userData, GCHandleType.Normal));

        return new()
        {
            Features = managed.DeviceFeatures,
            AdapterCallback = &AdapterFinderCallback,
            ValidationLayer = (uint)managed.ValidationType,
            Types = UnmanagedDescStreamType.ToUnmanaged(managed.Types, userData.Pool),
            Surface = UnmanagedRenderSurfaceDesc.ToUnmanaged(managed.Surface),
            UserData = handle
        };
    }

    public static void Free(Unmanaged unmanaged)
    {
        UnmanagedDescStreamType.Free(unmanaged.Types);
        UnmanagedRenderSurfaceDesc.Free(unmanaged.Surface);

        var handle = GCHandle.FromIntPtr(unmanaged.UserData);
        handle.Free();
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    private unsafe static uint AdapterFinderCallback(Unmanaged* unmanaged, UnmanagedGraphicsAdapterInfo* adapters, uint adapterCount)
    {
        uint adapterId = uint.MaxValue;
        var managed = GCHandle.FromIntPtr(unmanaged->UserData).Target as UserDataState;
        if (managed is not null && managed.Desc.AdapterFinder is not null)
        {
            var adapterList = new GraphicsAdapterInfo[adapterCount];
            for (uint i = 0; i < adapterCount; i++)
            {
                adapterList[i] = adapters[i].ToManaged();
            }
            adapterId = managed.Desc.AdapterFinder(adapterList) ?? adapterId;
        }
        return adapterId;
    }
}
