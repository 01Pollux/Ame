using System.Reflection;
using System.Runtime.InteropServices;
using AmeSharp.Core.Base;

namespace AmeSharp.Bridge.Core.Runtime
{
    public unsafe partial class AssemblyBridge
    {
        public static nint Create(Assembly assembly)
        {
            return GCHandleMarshaller<Assembly>.ConvertToUnmanaged(assembly);
        }

        [UnmanagedCallersOnly]
        public static UnmanagedNativeString GetName(nint library)
        {
            var assembly = GCHandleMarshaller<Assembly>.ConvertToManaged(library)!;
            return assembly!.GetName().Name;
        }

        [UnmanagedCallersOnly]
        public static UnmanagedNativeArray GetTypes(nint library)
        {
            var assembly = GCHandleMarshaller<Assembly>.ConvertToManaged(library)!;
            var types = assembly!.GetTypes();
            var typedUnmanaged = (TypedUnmanagedNativeArray<nint>)(types.Select(TypeBridge.Create).ToList());
            return typedUnmanaged.Array;
        }

        [UnmanagedCallersOnly]
        public static nint GetType(nint library, UnmanagedNativeString name)
        {
            var assembly = GCHandleMarshaller<Assembly>.ConvertToManaged(library)!;
            var type = assembly!.GetType(name!);
            return TypeBridge.Create(type);
        }
    }
}
