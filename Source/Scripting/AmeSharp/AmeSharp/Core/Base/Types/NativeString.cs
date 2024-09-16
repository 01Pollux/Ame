using AmeSharp.Bridge.Core.Base.Marshallers;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base.Types;

[StructLayout(LayoutKind.Sequential)]
public struct UnmanagedNativeString
{
    public nint Data;
    public ulong Size;

    public UnmanagedNativeString() => (Data, Size) = (nint.Zero, 0);
    public UnmanagedNativeString(nint data, ulong size) => (Data, Size) = (data, size);
    public UnmanagedNativeString(string value) => NativeStringMarshaller.ConvertToUnmanaged(value);

    public static implicit operator UnmanagedNativeString(string? value) => NativeStringMarshaller.ConvertToUnmanaged(value);
    public static implicit operator string?(UnmanagedNativeString value) => NativeStringMarshaller.ConvertToManaged(value);

    public readonly void Free() => NativeStringMarshaller.Free(this);
}
