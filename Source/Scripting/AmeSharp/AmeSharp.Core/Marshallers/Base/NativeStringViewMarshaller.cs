using AmeSharp.Core.Base.Types;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Text;

namespace AmeSharp.Core.Marshallers.Base;

[CustomMarshaller(typeof(NativeStringView), MarshalMode.Default, typeof(NativeStringViewMarshaller))]
[CustomMarshaller(typeof(NativeStringView), MarshalMode.ManagedToUnmanagedIn, typeof(ManagedToUnmanagedIn))]
public static unsafe class NativeStringViewMarshaller
{
    public unsafe struct Unmanaged
    {
        public byte* Bytes;
        public ulong Length;
    }

    public static Unmanaged ConvertToUnmanaged(NativeStringView? managed)
    {
        Unmanaged unmanaged = new();
        if (managed is null)
        {
            return unmanaged;
        }

        int exactByteCount = checked(Encoding.UTF8.GetByteCount(managed.Value) + 1);
        unmanaged.Length = (ulong)exactByteCount;
        unmanaged.Bytes = (byte*)Marshal.AllocCoTaskMem(exactByteCount);

        Span<byte> buffer = new(unmanaged.Bytes, exactByteCount);

        int byteCount = Encoding.UTF8.GetBytes(managed.Value, buffer);
        buffer[byteCount] = 0;
        return unmanaged;
    }

    public static NativeStringView ConvertToManaged(Unmanaged unmanaged)
    {
        return new(Marshal.PtrToStringUTF8((nint)unmanaged.Bytes, (int)unmanaged.Length));
    }

    public static void Free(Unmanaged unmanaged)
    {
        if (unmanaged.Bytes != null)
        {
            NativeMemory.Free(unmanaged.Bytes);
        }
    }

    public ref struct ManagedToUnmanagedIn
    {
        public static int BufferSize => 0x100;

        Unmanaged _view;
        bool _allocated;

        public void FromManaged(NativeStringView? managed, Span<byte> buffer)
        {
            _allocated = false;

            if (managed is null)
            {
                _view.Bytes = null;
                _view.Length = 0;
                return;
            }

            const int MaxUtf8BytesPerChar = 3;

            // >= for null terminator
            // Use the cast to long to avoid the checked operation
            if ((long)MaxUtf8BytesPerChar * managed.Value.Length >= buffer.Length)
            {
                // Calculate accurate byte count when the provided stack-allocated buffer is not sufficient
                int exactByteCount = checked(Encoding.UTF8.GetByteCount(managed.Value) + 1); // + 1 for null terminator
                if (exactByteCount > buffer.Length)
                {
                    buffer = new Span<byte>((byte*)NativeMemory.Alloc((nuint)exactByteCount), exactByteCount);
                    _allocated = true;
                }
            }

            int byteCount = Encoding.UTF8.GetBytes(managed.Value, buffer);
            buffer[byteCount] = 0;

            _view.Bytes = (byte*)Unsafe.AsPointer(ref MemoryMarshal.GetReference(buffer));
            _view.Length = (ulong)byteCount;
        }

        public readonly Unmanaged ToUnmanaged() => _view;

        public void Free()
        {
            if (_allocated)
            {
                NativeMemory.Free(_view.Bytes);
                _allocated = false;
            }
        }
    }
}
