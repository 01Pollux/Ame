
using AmeSharp.Core.Base;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Runtime
{
    public unsafe partial class AssemblyBridge
    {
        [UnmanagedCallersOnly]
        public static UnmanagedNativeString GetName(nint library)
        {
            var assembly = GCHandleMarshaller<Assembly>.ConvertToManaged(library)!;
            return assembly.GetName().Name;
        }

        public static void GetTypes(nint library)
        {
            var assembly = GCHandleMarshaller<Assembly>.ConvertToManaged(library)!;
            if (assembly == null)
            {
                return;
            }

            var types = assembly.GetTypes();
        }
    }
}
