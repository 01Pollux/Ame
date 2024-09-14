using AmeSharp.Core.Base.Marshallers;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base
{
    public record NativeArray<T>(List<T> Values)
    {
        public static implicit operator NativeArray<T>(List<T> list) => new(list);
        public static implicit operator NativeArray<T>(T[] list) => new(list);
        public static implicit operator List<T>(NativeArray<T> list) => list.Values;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct UnmanagedNativeArray 
    {
        public nint Data;
        public ulong Size;
    }


    [StructLayout(LayoutKind.Sequential)]
    public struct TypedUnmanagedNativeArray<T> where T : unmanaged
    {
        public UnmanagedNativeArray Array;

        public static implicit operator TypedUnmanagedNativeArray<T>(NativeArray<T> value) => NativeArrayMarshaller<T>.ConvertToUnmanaged(value);
        public static implicit operator NativeArray<T>?(TypedUnmanagedNativeArray<T> value) => NativeArrayMarshaller<T>.ConvertToManaged(value);
        public static implicit operator TypedUnmanagedNativeArray<T>(List<T> value) => NativeArrayMarshaller<T>.ConvertToUnmanaged(new(value));
        public static implicit operator List<T>?(TypedUnmanagedNativeArray<T> value) => NativeArrayMarshaller<T>.ConvertToManaged(value)?.Values;
        public static void Free(TypedUnmanagedNativeArray<T> value) => NativeArrayMarshaller<T>.Free(value);
    }
}
