using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Runtime
{
    public unsafe partial class InstanceBridge
    {
        public static nint Create(object? obj)
        {
            return obj is null ? nint.Zero : GCHandleMarshaller<object>.ConvertToUnmanaged(obj);
        }

        [UnmanagedCallersOnly]
        public static void Free(nint typePtr)
        {
            GCHandleMarshaller<object>.Free(typePtr);
        }
    }
}
