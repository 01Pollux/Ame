using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base;

internal partial class RawDataBlobBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_RawDataBlob_CreateDataBlob")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr FromBytes(IntPtr data, ulong dataSize);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_RawDataBlob_CreateDataBlobFromBlob")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr FromBlob(IntPtr blobObject);

    public static IntPtr FromBytes(byte[] data)
    {
        unsafe
        {
            fixed (byte* ptr = data)
            {
                return FromBytes((IntPtr)ptr, (ulong)data.Length);
            }
        }
    }

}
