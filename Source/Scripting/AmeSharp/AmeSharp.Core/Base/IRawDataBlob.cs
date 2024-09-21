using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("CD902203-B43C-48CA-9C6A-625AC4C98D5B")]
public class IRawDataBlob : IDataBlob
{
    public IRawDataBlob(nint obj) : base(obj, true) { }
    public IRawDataBlob(nint data, ulong dataSize) : base(RawDataBlobBridge.FromBytes(data, dataSize), false) { }
    public IRawDataBlob(byte[] data) : base(RawDataBlobBridge.FromBytes(data), false) { }
    public IRawDataBlob(IDataBlob otherData) : base(RawDataBlobBridge.FromBlob(otherData.NativePointer), false) { }
}
