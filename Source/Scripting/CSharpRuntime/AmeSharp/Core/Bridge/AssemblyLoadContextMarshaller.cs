using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Runtime.Loader;

namespace AmeSharp.Core.Bridge
{
    [CustomMarshaller(typeof(AssemblyLoadContext), MarshalMode.UnmanagedToManagedIn, typeof(AssemblyLoadContextMarshaller))]
    [CustomMarshaller(typeof(AssemblyLoadContext), MarshalMode.ManagedToUnmanagedIn, typeof(AssemblyLoadContextMarshaller))]
    internal static unsafe class AssemblyLoadContextMarshaller
    {
        public static AssemblyLoadContext? ConvertToManaged(IntPtr unmanaged)
        {
            var handle = (GCHandle)unmanaged;
            return handle.Target as AssemblyLoadContext;
        }

        public static IntPtr ConvertToUnmanaged(AssemblyLoadContext managed)
        {
            var handle = GCHandle.Alloc(managed);
            return (IntPtr)handle;
        }

        public static void Free(IntPtr unmanaged)
        {
            if (unmanaged != IntPtr.Zero)
            {
                GCHandle handle = (GCHandle)unmanaged;
                handle.Free();
            }
        }
    }
}
