using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base
{
    internal unsafe partial class BaseObjectBridge
    {
        [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_BaseObject_QueryInterface")]
        [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
        public static partial void QueryInterface(IntPtr thisObject, ref Guid iid, ref IntPtr output);

        [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
        [DllImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_BaseObject_AddRef")]
        public static extern void AddRef(IntPtr thisObject);

        [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_BaseObject_Release")]
        [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
        public static partial void Release(IntPtr thisObject);
    }
}
