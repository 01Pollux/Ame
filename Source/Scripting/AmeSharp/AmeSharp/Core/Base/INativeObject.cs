using AmeSharp.Core.Internal;

namespace AmeSharp.Core.Base;

public class INativeObject : IDisposable
{
    public virtual IntPtr NativePointer { get; protected set; }

    public INativeObject() { }
    public INativeObject(IntPtr nativePointer)
    {
        IAbstractStorage.Set(nativePointer, this);
        NativePointer = nativePointer;
    }

    ~INativeObject() => Dispose(false);

    //

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    //

    protected virtual void Dispose(bool disposing)
    {
        if (NativePointer != IntPtr.Zero)
        {
            IAbstractStorage.Remove(NativePointer);
            NativePointer = IntPtr.Zero;
        }
    }
}
