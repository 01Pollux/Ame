using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("99E53E10-55D1-4E81-871D-09C580060D43")]
public abstract class IDataBlob(IntPtr handle) : IBaseObject(handle, true, false)
{
    public ulong Size
    {
        get => DataBlobBridge.GetSize(Handle);
        set => DataBlobBridge.Resize(Handle, value);
    }

    public unsafe byte* Data => (byte*)DataBlobBridge.GetData(Handle);
    public unsafe byte* ConstData => (byte*)DataBlobBridge.GetConstData(Handle);
}
