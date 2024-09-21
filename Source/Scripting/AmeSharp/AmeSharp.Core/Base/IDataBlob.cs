using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("99E53E10-55D1-4E81-871D-09C580060D43")]
public class IDataBlob : IBaseObject
{
    public IDataBlob(nint obj, bool isExternal) : base(obj, isExternal) { }

    public ulong Size
    {
        get => DataBlobBridge.GetSize(NativePointer);
        set => DataBlobBridge.Resize(NativePointer, value);
    }

    public unsafe byte* Data => (byte*)DataBlobBridge.GetData(NativePointer);
    public unsafe byte* ConstData => (byte*)DataBlobBridge.GetConstData(NativePointer);
}
