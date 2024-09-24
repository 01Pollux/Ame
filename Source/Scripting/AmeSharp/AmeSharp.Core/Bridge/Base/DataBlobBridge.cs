using AmeSharp.Core.Base;
using AmeSharp.Core.Bridge;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base;

internal partial class DataBlobBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IDataBlob_Resize")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Resize(IntPtr blob, ulong size);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IDataBlob_GetSize")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial ulong GetSize(IntPtr blob);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IDataBlob_GetData")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetData(IntPtr blob);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IDataBlob_GetConstData")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetConstData(IntPtr blob);
}
