using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("CD902203-B43C-48CA-9C6A-625AC4C98D5B")]
public class IRawDataBlob : IDataBlob
{
    private IRawDataBlob(IntPtr handle) : base(handle) { }

    public static IDataBlob FromBytes(nint data, ulong dataSize) => new IRawDataBlob(RawDataBlobBridge.FromBytes(data, dataSize));
    public static IDataBlob FromBytes(byte[] data) => new IRawDataBlob(RawDataBlobBridge.FromBytes(data));
    public static IDataBlob FromStream(Stream stream)
    {
        using var memoryStream = new MemoryStream();
        stream.CopyTo(memoryStream);
        return FromBytes(memoryStream.ToArray());
    }
    public static IDataBlob FromBlob(IDataBlob otherData) => new IRawDataBlob(RawDataBlobBridge.FromBlob(otherData.Handle));
}
