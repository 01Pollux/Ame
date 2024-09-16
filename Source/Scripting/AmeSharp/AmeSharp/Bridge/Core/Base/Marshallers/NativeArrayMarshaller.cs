using AmeSharp.Core.Base.Types;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base.Marshallers;

internal static unsafe class NativeArrayMarshaller<T> where T : unmanaged
{
    public static nint Allocate(ulong size)
    {
        int byteCount = checked(Marshal.SizeOf<T>() * (int)size);
        return (nint)NativeMemory.Alloc((nuint)byteCount);
    }

    public static RawUnmanagedNativeArray ConvertToUnmanaged(T[]? managed)
    {
        var unmanaged = new RawUnmanagedNativeArray();
        if (managed is null)
        {
            return unmanaged;
        }

        int byteCount = checked(Marshal.SizeOf<T>() * managed.Length);
        unmanaged.Data = Allocate((ulong)managed.Length);
        unmanaged.Length = (ulong)managed.Length;

        fixed (T* p = managed)
        {
            byteCount = Marshal.SizeOf<T>() * managed.Length;
            Buffer.MemoryCopy(p, (void*)unmanaged.Data, byteCount, byteCount);
        }

        return unmanaged;
    }

    public static T[]? ConvertToManaged(RawUnmanagedNativeArray unmanaged)
    {
        if (unmanaged.Data == nint.Zero)
        {
            return null;
        }

        int byteCount = checked(Marshal.SizeOf<T>() * (int)unmanaged.Length);
        T[] values = new T[unmanaged.Length];
        fixed (T* p = values)
        {
            Buffer.MemoryCopy((void*)unmanaged.Data, p, byteCount, byteCount);
        }
        return values;
    }

    public static void Free(ref UnmanagedNativeArray<T> unmanaged)
    {
        if (unmanaged.Data != nint.Zero)
        {
            NativeMemory.Free((void*)unmanaged.Data);
            unmanaged = new();
        }
    }
}
