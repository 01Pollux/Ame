using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Text;

namespace AmeSharp.Core.Base.Marshallers
{
    [CustomMarshaller(typeof(string), MarshalMode.ManagedToUnmanagedOut, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(string), MarshalMode.ManagedToUnmanagedIn, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(string), MarshalMode.UnmanagedToManagedIn, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(string), MarshalMode.UnmanagedToManagedOut, typeof(NativeStringMarshaller))]
    internal static unsafe class NativeStringMarshaller
    {
        public static UnmanagedNativeString ConvertToUnmanaged(string? managed)
        {
            if (managed is null)
            {
                return new();
            }

            int byteCount = checked(Encoding.UTF8.GetByteCount(managed) + 1);
            UnmanagedNativeString unmanaged = new(
                (nint)NativeMemory.Alloc((nuint)byteCount),
                (ulong)byteCount
            );

            fixed (char* p = managed)
            {
                byteCount = Encoding.UTF8.GetBytes(p, managed.Length, (byte*)unmanaged.Data, byteCount);
                ((byte*)unmanaged.Data)[byteCount] = 0;
            }

            return unmanaged;
        }

        public static string? ConvertToManaged(UnmanagedNativeString unmanaged)
        {
            return unmanaged.Data == nint.Zero ? null : Marshal.PtrToStringUTF8(unmanaged.Data, (int)unmanaged.Size);
        }

        public static void Free(UnmanagedNativeString unmanaged)
        {
            if (unmanaged.Data != nint.Zero)
            {
                NativeMemory.Free((void*)unmanaged.Data);
            }
        }

    }
}
