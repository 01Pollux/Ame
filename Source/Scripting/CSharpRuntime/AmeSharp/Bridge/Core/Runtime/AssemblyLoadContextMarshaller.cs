using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Runtime.Loader;

namespace AmeSharp.Bridge.Core.Runtime
{
    [CustomMarshaller(typeof(AssemblyLoadContext), MarshalMode.UnmanagedToManagedIn, typeof(AssemblyLoadContextMarshaller))]
    [CustomMarshaller(typeof(AssemblyLoadContext), MarshalMode.ManagedToUnmanagedIn, typeof(AssemblyLoadContextMarshaller))]
    internal static unsafe class AssemblyLoadContextMarshaller
    {
        public static AssemblyLoadContext? ConvertToManaged(nint unmanaged)
        {
            var handle = (GCHandle)unmanaged;
            return handle.Target as AssemblyLoadContext;
        }

        public static nint ConvertToUnmanaged(AssemblyLoadContext managed)
        {
            var handle = GCHandle.Alloc(managed);
            return (nint)handle;
        }

        public static void Free(nint unmanaged)
        {
            if (unmanaged != nint.Zero)
            {
                GCHandle handle = (GCHandle)unmanaged;
                handle.Free();
            }
        }
    }
}
