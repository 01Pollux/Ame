using AmeSharp.Runtime.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Runtime.Core.Bridge
{
    public unsafe partial class BridgeAssemblyLoadContext
    {
        [UnmanagedCallersOnly]
        public static IntPtr CreateAssemblyLoadContext(IntPtr name)
        {
            var nativeString = Marshal.PtrToStructure<NativeString>(name) ?? NativeString.Empty;
            Console.WriteLine(nativeString);
            return IntPtr.Zero;
        }
    }
}
