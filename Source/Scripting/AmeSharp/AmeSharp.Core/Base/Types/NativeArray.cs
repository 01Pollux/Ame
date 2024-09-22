using AmeSharp.Core.Marshallers.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base.Types;

public struct RawUnmanagedNativeArray
{
    public nint Data;
    public ulong Length;

    public RawUnmanagedNativeArray(nint data, ulong length) => (Data, Length) = (data, length);
}

[StructLayout(LayoutKind.Sequential)]
public struct UnmanagedNativeArray<T> where T : unmanaged
{
    public RawUnmanagedNativeArray Storage;

    public readonly nint Data => Storage.Data;
    public readonly ulong Length => Storage.Length;

    public unsafe readonly T this[ulong index]
    {
        get => Marshal.PtrToStructure<T>(Data + (nint)(index * (ulong)Marshal.SizeOf<T>()));
        set => Marshal.StructureToPtr(value, Data + (nint)(index * (ulong)Marshal.SizeOf<T>()), false);
    }

    public UnmanagedNativeArray(RawUnmanagedNativeArray storage) => Storage = storage;
    public UnmanagedNativeArray(nint data, ulong size) => (Storage.Data, Storage.Length) = (data, size);
    public UnmanagedNativeArray(T[]? values) => Storage = NativeArrayMarshaller<T>.ConvertToUnmanaged(values);
    public UnmanagedNativeArray(IEnumerable<T> values) => Storage = NativeArrayMarshaller<T>.ConvertToUnmanaged(values.ToArray());
    public UnmanagedNativeArray(ulong size) => (Storage.Data, Storage.Length) = (NativeArrayMarshaller<T>.Allocate(size), size);

    public void Free() => NativeArrayMarshaller<T>.Free(ref this);

    public readonly T[] ToArray()
    {
        if (Data == nint.Zero)
        {
            return [];
        }

        var arr = new T[Length];
        for (ulong i = 0; i < Length; i++)
        {
            arr[i] = Marshal.PtrToStructure<T>(Data + (nint)(i * (ulong)Marshal.SizeOf<T>()));
        }
        return arr;
    }
}
