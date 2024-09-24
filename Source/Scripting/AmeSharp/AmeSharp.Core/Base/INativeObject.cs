using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

public abstract class INativeObject(IntPtr handle, bool ownsHandle) : SafeHandle(handle, ownsHandle)
{
    public static IntPtr InvalidHandle => IntPtr.Zero;
    public static bool IsHandleInvalid(IntPtr handle) => handle == InvalidHandle;

    public IntPtr Handle => IsInvalid ? throw new ObjectDisposedException(nameof(INativeObject)) : handle;

    public override bool IsInvalid => IsHandleInvalid(handle);
}
