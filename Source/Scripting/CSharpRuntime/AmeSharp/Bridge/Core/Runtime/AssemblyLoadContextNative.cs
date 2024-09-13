using AmeSharp.Core.Base;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace AmeSharp.Bridge.Core.Runtime
{
    public unsafe partial class ALCBridge
    {
        [UnmanagedCallersOnly]
        public static nint Create(UnmanagedNativeString name)
        {
            AssemblyLoadContext context = new(name!, true);
            return AssemblyLoadContextMarshaller.ConvertToUnmanaged(context);
        }

        [UnmanagedCallersOnly]
        public static void Unload(nint contextPtr)
        {
            AssemblyLoadContext context = AssemblyLoadContextMarshaller.ConvertToManaged(contextPtr)!;
            context.Unload();
            AssemblyLoadContextMarshaller.Free(contextPtr);
        }

        [UnmanagedCallersOnly]
        public static void LoadLibrary(nint contextPtr, UnmanagedNativeString path)
        {
            AssemblyLoadContext context = AssemblyLoadContextMarshaller.ConvertToManaged(contextPtr)!;
            context.LoadFromAssemblyPath(path!);
        }

        [UnmanagedCallersOnly]
        public static void LoadLibraryFrom(nint contextPtr, UnmanagedNativeString path)
        {
            AssemblyLoadContext context = AssemblyLoadContextMarshaller.ConvertToManaged(contextPtr)!;
            context.LoadFromAssemblyPath(path!);
        }
    }
}
