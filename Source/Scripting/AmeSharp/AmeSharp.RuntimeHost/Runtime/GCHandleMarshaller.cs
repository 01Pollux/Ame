using System.Runtime.InteropServices;

namespace AmeSharp.RuntimeHost.Runtime;

internal static unsafe class GCHandleMarshaller<T> where T : class
{
    public static T? ConvertToManaged(nint unmanaged)
    {
        var handle = (GCHandle)unmanaged;
        return handle.Target as T;
    }

    public static nint ConvertToUnmanaged(T managed)
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
