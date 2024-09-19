namespace AmeSharp.Core.Base;

public abstract class ISignalConnection(IntPtr obj) : INativeObject(obj)
{
    public abstract void Disconnect();
}
