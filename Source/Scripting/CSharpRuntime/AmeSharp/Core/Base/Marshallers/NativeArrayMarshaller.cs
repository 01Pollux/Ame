using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base.Marshallers
{
    internal static unsafe class NativeArrayMarshaller<T> where T : unmanaged
    {
        public static TypedUnmanagedNativeArray<T> ConvertToUnmanaged(NativeArray<T> managed)
        {
            var array = new UnmanagedNativeArray { Data = nint.Zero, Size = 0 };
            var unmanaged = new TypedUnmanagedNativeArray<T> { Array = array };
            if (managed.Values == null)
            {
                return unmanaged;
            }

            int byteCount = checked(Marshal.SizeOf<T>() * managed.Values.Count);
            unmanaged.Array.Data = (nint)NativeMemory.Alloc((nuint)byteCount);
            unmanaged.Array.Size = (ulong)managed.Values.Count;

            fixed (T* p = managed.Values.ToArray())
            {
                byteCount = Marshal.SizeOf<T>() * managed.Values.Count;
                Buffer.MemoryCopy(p, (void*)unmanaged.Array.Data, byteCount, byteCount);
            }

            return unmanaged;
        }

        public static NativeArray<T>? ConvertToManaged(TypedUnmanagedNativeArray<T> unmanaged)
        {
            if (unmanaged.Array.Data == nint.Zero)
            {
                return null;
            }
            
            int byteCount = checked(Marshal.SizeOf<T>() * (int)unmanaged.Array.Size);
            List<T> values = new();
            values.Capacity = (int)unmanaged.Array.Size / Marshal.SizeOf<T>();
            fixed (T* p = values.ToArray())
            {
                Buffer.MemoryCopy((void*)unmanaged.Array.Data, p, byteCount, byteCount);
            }
            return new(values);
        }

        public static void Free(TypedUnmanagedNativeArray<T> unmanaged)
        {
            if (unmanaged.Array.Data != nint.Zero)
            {
                NativeMemory.Free((void*)unmanaged.Array.Data);
            }
        }
    }
}
