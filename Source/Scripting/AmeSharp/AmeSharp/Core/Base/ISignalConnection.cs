namespace AmeSharp.Core.Base;

public abstract class ISignalConnection(IntPtr obj) : INativeObject(obj)
{
    public delegate void SignalCallback();

    public abstract void Disconnect();
}
