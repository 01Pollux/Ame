using AmeSharp.Core.Base.Marshallers;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Base
{
    [NativeMarshalling(typeof(NativeStringMarshaller))]
    public record NativeString(string Value)
    {
        public static implicit operator NativeString(string value) => new(value);
        public static implicit operator string(NativeString value) => value.Value;
        public override string ToString() => Value;

        public static NativeString Empty { get; } = new(string.Empty);
    }

    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct UnmanagedNativeString
    {
        public nint Data;
        public ulong Size;

        public static implicit operator UnmanagedNativeString(NativeString value) => NativeStringMarshaller.ConvertToUnmanaged(value);
        public static implicit operator NativeString?(UnmanagedNativeString value) => NativeStringMarshaller.ConvertToManaged(value);
        public static implicit operator UnmanagedNativeString(string? value) => NativeStringMarshaller.ConvertToUnmanaged(new(value ?? string.Empty));
        public static implicit operator string?(UnmanagedNativeString value) => NativeStringMarshaller.ConvertToManaged(value)?.Value;

        public static void Free(UnmanagedNativeString value) => NativeStringMarshaller.Free(value);

        public override readonly string? ToString() => NativeStringMarshaller.ConvertToManaged(this)?.Value;

        public readonly UnmanagedNativeString Empty => new() { Data = nint.Zero, Size = 0 };
    }
}
