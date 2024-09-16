namespace AmeSharp.Core.Base;

public class INativeObject(IntPtr obj) : IDisposable
{
    public IntPtr NativePointer { get; private set; } = obj;

    ~INativeObject() => Dispose(false);

    //

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    protected virtual void Dispose(bool disposing)
    {
        if (NativePointer != IntPtr.Zero)
        {
            NativePointer = IntPtr.Zero;
        }
    }
}
