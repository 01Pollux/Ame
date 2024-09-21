using AmeSharp.Core.Bridge.Internal;

namespace AmeSharp.Core.Base;

/// <summary>
/// Native object base class.
/// Represents a native object in C++ side.
/// 
/// Each native object should inherit this class and MUST implement a constructor that takes a pointer to the native object.
/// Inherited classes can also implement their own constructor(s) if needed.
/// 
/// Instances that are created with obj pointer are automatically stored in a dictionary.
/// If however, we tried to get the instance of the object with the same pointer, without creating a new instance for it, it will create a new REFERENCE instance and store it for future use.
/// Keep in mind that those created references are only, well references, and one should be careful when accessing properties/fields/functions of the object.
/// </summary>
public class INativeObject : IDisposable
{
    public virtual nint NativePointer { get; private set; }

    public static INativeObject? Get(nint nativePointer) => NativeObjectStorage.Get(nativePointer);
    public static T? Get<T>(nint nativePointer) where T : INativeObject => NativeObjectStorage.Get<T>(nativePointer);

    public INativeObject(nint nativePointer)
    {
        try
        {
            NativeObjectStorage.Set(nativePointer, this);
        }
        catch
        {
            Dispose();
            throw;
        }
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
        if (NativePointer != nint.Zero)
        {
            NativeObjectStorage.Remove(NativePointer);
            NativePointer = nint.Zero;
        }
    }
}
