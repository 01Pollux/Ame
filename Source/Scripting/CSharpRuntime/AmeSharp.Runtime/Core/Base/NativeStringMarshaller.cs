using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Text;

namespace AmeSharp.Runtime.Core.Base
{
    [CustomMarshaller(typeof(NativeString), MarshalMode.ManagedToUnmanagedOut, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(NativeString), MarshalMode.ManagedToUnmanagedIn, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(NativeString), MarshalMode.UnmanagedToManagedIn, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(NativeString), MarshalMode.UnmanagedToManagedOut, typeof(NativeStringMarshaller))]
    internal static unsafe class NativeStringMarshaller
    {
        internal struct Unmanaged
        {
            public IntPtr Data;
            public ulong Size;
        }

        public static Unmanaged ConvertToUnmanaged(NativeString managed)
        {
            Unmanaged unmanaged = new()
            {
                Size = checked((ulong)(Encoding.UTF8.GetByteCount(managed.Value) + 1))
            };

            unmanaged.Data = (IntPtr)NativeMemory.AllocZeroed((nuint)unmanaged.Size);
            fixed (char* p = managed.Value)
            {
                Encoding.UTF8.GetBytes(p, managed.Value.Length, (byte*)unmanaged.Data, (int)unmanaged.Size);
            }

            return unmanaged;
        }

        public static NativeString ConvertToManaged(Unmanaged unmanaged)
        {
            return new(Encoding.UTF8.GetString((byte*)unmanaged.Data, (int)unmanaged.Size));
        }

        public static void Free(Unmanaged unmanaged)
        {
            NativeMemory.Free((void*)unmanaged.Data);
        }
    }
}
