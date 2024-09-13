using AmeSharp.Core.Base;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace AmeSharp.Core.Bridge
{
    public unsafe partial class AssemblyLoadContextNative
    {
        [UnmanagedCallersOnly(EntryPoint = "AssemblyLoadContext_Create")]
        public static IntPtr Create(UnmanagedNativeString name)
        {
            AssemblyLoadContext context = new(name!);
            return AssemblyLoadContextMarshaller.ConvertToUnmanaged(context);
        }

        [UnmanagedCallersOnly(EntryPoint = "AssemblyLoadContext_Unload")]
        public static void Unload(IntPtr contextPtr)
        {
            AssemblyLoadContext context = AssemblyLoadContextMarshaller.ConvertToManaged(contextPtr)!;
            context.Unload();
            AssemblyLoadContextMarshaller.Free(contextPtr);
        }

        [UnmanagedCallersOnly(EntryPoint = "AssemblyLoadContext_Load")]
        public static void Load(IntPtr contextPtr, UnmanagedNativeString path)
        {
            AssemblyLoadContext context = AssemblyLoadContextMarshaller.ConvertToManaged(contextPtr)!;
            context.LoadFromAssemblyPath(path!);
        }

        [UnmanagedCallersOnly(EntryPoint = "AssemblyLoadContext_LoadFrom")]
        public static void LoadFrom(IntPtr contextPtr, UnmanagedNativeString path)
        {
            AssemblyLoadContext context = AssemblyLoadContextMarshaller.ConvertToManaged(contextPtr)!;
            context.LoadFromAssemblyPath(path!);
        }
    }
}
