using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("74063704-A586-47A5-9349-DE2AC9370DFC")]
public class IRawDataBlob : IDataBlob
{
    public IRawDataBlob(IntPtr obj) : base(obj) { }
    public IRawDataBlob(IntPtr data, ulong dataSize) : base(RawDataBlobBridge.FromBytes(data, dataSize)) { }
    public IRawDataBlob(byte[] data) : base(RawDataBlobBridge.FromBytes(data)) { }
    public IRawDataBlob(IDataBlob otherData) : base(RawDataBlobBridge.FromBlob(otherData.NativePointer)) { }
}
