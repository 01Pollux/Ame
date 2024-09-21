using AmeSharp.Core.Base.Types;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.RuntimeHost.Runtime;

public unsafe partial class MethodBridge
{
    public static nint Create(MethodInfo? method)
    {
        return method is null ? nint.Zero : GCHandleMarshaller<MethodInfo>.ConvertToUnmanaged(method);
    }
    public static MethodInfo? Get(nint methodPtr)
    {
        return GCHandleMarshaller<MethodInfo>.ConvertToManaged(methodPtr);
    }

    [UnmanagedCallersOnly]
    public static void Free(nint methodPtr)
    {
        GCHandleMarshaller<MethodInfo>.Free(methodPtr);
    }

    [UnmanagedCallersOnly]
    public static bool IsStatic(nint methodPtr)
    {
        var method = Get(methodPtr);
        return method?.IsStatic ?? false;
    }

    [UnmanagedCallersOnly]
    public static UnmanagedNativeString GetName(nint methodPtr)
    {
        var method = Get(methodPtr);
        return method?.Name ?? string.Empty;
    }

    [UnmanagedCallersOnly]
    public static RawUnmanagedNativeArray GetParamTypes(nint methodPtr)
    {
        var method = Get(methodPtr);
        return Marshalling.ParamsToPtr(method).Storage;
    }

    [UnmanagedCallersOnly]
    public static nint GetReturnType(nint methodPtr)
    {
        var method = Get(methodPtr);
        return TypeBridge.Create(method?.ReturnType);
    }

    [UnmanagedCallersOnly]
    public static void Invoke(nint methodPtr, nint obj, nint args, ulong argCount, nint retPtr)
    {
        var method = Get(methodPtr)!;
        var arguments = Marshalling.PtrToParams(method, args, argCount);

        object? thisPtr = obj == nint.Zero ? null : InstanceBridge.Get(obj);
        var ret = method.Invoke(thisPtr, arguments);
        if (retPtr != nint.Zero)
        {
            if (ret is null || ret.GetType() == typeof(void))
            {
                Marshal.WriteIntPtr(retPtr, nint.Zero);
            }
            else
            {
                Marshal.StructureToPtr(ret, retPtr, true);
            }
        }
    }
}
