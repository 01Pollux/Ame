namespace AmeSharp.Core.Utils.Callbacks;

public static class CallbackUtils
{
    public delegate void Callback();
    public unsafe delegate nint CallbackRegister(delegate* unmanaged[Cdecl]<nint, void> callbackImpl, nint thisHandle);
}
