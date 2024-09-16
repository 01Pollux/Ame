using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("F578FF0D-ABD2-4514-9D32-7CB454D4A73B")]
public class IDataBlob(IntPtr obj) : IBaseObject(obj)
{
    public ulong Size
    {
        get => DataBlobBridge.GetSize(NativePointer);
        set => DataBlobBridge.Resize(NativePointer, value);
    }

    public unsafe byte* Data => (byte*)DataBlobBridge.GetData(NativePointer);
    public unsafe byte* ConstData => (byte*)DataBlobBridge.GetConstData(NativePointer);
}
