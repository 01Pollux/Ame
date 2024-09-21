using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("CD902203-B43C-48CA-9C6A-625AC4C98D5B")]
public class IRawDataBlob : IDataBlob
{
    public IRawDataBlob(IntPtr obj) : base(obj) { }
    public IRawDataBlob(IntPtr data, ulong dataSize) : base(RawDataBlobBridge.FromBytes(data, dataSize)) { }
    public IRawDataBlob(byte[] data) : base(RawDataBlobBridge.FromBytes(data)) { }
    public IRawDataBlob(IDataBlob otherData) : base(RawDataBlobBridge.FromBlob(otherData.NativePointer)) { }
}
