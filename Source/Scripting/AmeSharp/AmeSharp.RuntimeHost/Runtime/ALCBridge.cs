using AmeSharp.Core.Base.Types;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace AmeSharp.RuntimeHost.Runtime;

public unsafe partial class ALCBridge
{
    [UnmanagedCallersOnly]
    public static nint Create(UnmanagedNativeString name)
    {
        ALCWrapper context = new(new AssemblyLoadContext(name!, true));
        return GCHandleMarshaller<ALCWrapper>.ConvertToUnmanaged(context);
    }

    [UnmanagedCallersOnly]
    public static void Unload(nint contextPtr)
    {
        var context = GCHandleMarshaller<ALCWrapper>.ConvertToManaged(contextPtr)!;
        context.UnloadContext();
    }

    [UnmanagedCallersOnly]
    public static nint LoadLibrary(nint contextPtr, UnmanagedNativeString path)
    {
        var context = GCHandleMarshaller<ALCWrapper>.ConvertToManaged(contextPtr)!;
        string pathStr = path!;
        if (!Path.IsPathFullyQualified(pathStr))
        {
            pathStr = Path.GetFullPath(pathStr);
        }
        return context.LoadAssembly(pathStr);
    }

    [UnmanagedCallersOnly]
    public static nint LoadLibraryFromStream(nint contextPtr, byte* data, long size)
    {
        var context = GCHandleMarshaller<ALCWrapper>.ConvertToManaged(contextPtr)!;
        using UnmanagedMemoryStream stream = new(data, size);
        return context.LoadAssembly(stream);
    }
}
