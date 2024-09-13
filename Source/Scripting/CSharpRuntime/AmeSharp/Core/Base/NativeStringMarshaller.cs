using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Text;

namespace AmeSharp.Core.Base
{
    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct UnmanagedNativeString
    {
        public IntPtr Data;
        public ulong Size;

        public static implicit operator UnmanagedNativeString(NativeString value) => NativeStringMarshaller.ConvertToUnmanaged(value);
        public static implicit operator NativeString?(UnmanagedNativeString value) => NativeStringMarshaller.ConvertToManaged(value);
        public static implicit operator UnmanagedNativeString(string? value) => NativeStringMarshaller.ConvertToUnmanaged(new(value ?? string.Empty));
        public static implicit operator string?(UnmanagedNativeString value) => NativeStringMarshaller.ConvertToManaged(value)?.Value;

        public static void Free(UnmanagedNativeString value) => NativeStringMarshaller.Free(value);

        public override readonly string? ToString() => NativeStringMarshaller.ConvertToManaged(this)?.Value;

        public readonly UnmanagedNativeString Empty => new() { Data = IntPtr.Zero, Size = 0 };
    }

    [CustomMarshaller(typeof(NativeString), MarshalMode.ManagedToUnmanagedOut, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(NativeString), MarshalMode.ManagedToUnmanagedIn, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(NativeString), MarshalMode.UnmanagedToManagedIn, typeof(NativeStringMarshaller))]
    [CustomMarshaller(typeof(NativeString), MarshalMode.UnmanagedToManagedOut, typeof(NativeStringMarshaller))]
    internal static unsafe class NativeStringMarshaller
    {
        public static UnmanagedNativeString ConvertToUnmanaged(NativeString? managed)
        {
            if (managed == null)
            {
                return new UnmanagedNativeString { Data = IntPtr.Zero, Size = 0 };
            }

            int byteCount = checked(Encoding.UTF8.GetByteCount(managed.Value) + 1);
            UnmanagedNativeString unmanaged = new()
            {
                Data = (nint)NativeMemory.Alloc((nuint)byteCount),
                Size = (ulong)byteCount
            };

            fixed (char* p = managed.Value)
            {
                byteCount = Encoding.UTF8.GetBytes(p, managed.Value.Length, (byte*)unmanaged.Data, byteCount);
                ((byte*)unmanaged.Data)[byteCount] = 0;
            }

            return unmanaged;
        }

        public static NativeString? ConvertToManaged(UnmanagedNativeString unmanaged)
        {
            return unmanaged.Data == IntPtr.Zero ? null : new(Marshal.PtrToStringUTF8(unmanaged.Data, (int)unmanaged.Size));
        }

        public static void Free(UnmanagedNativeString unmanaged)
        {
            if (unmanaged.Data != IntPtr.Zero)
            {
                NativeMemory.Free((void*)unmanaged.Data);
            }
        }
    }
}
