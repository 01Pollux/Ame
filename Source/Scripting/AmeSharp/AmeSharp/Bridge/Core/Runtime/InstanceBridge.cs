using AmeSharp.Core.Base.Types;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Runtime;

public unsafe partial class InstanceBridge
{
    public static nint Create(object? obj)
    {
        return obj is null ? nint.Zero : GCHandleMarshaller<object>.ConvertToUnmanaged(obj);
    }
    public static object? Get(nint instancePtr)
    {
        return GCHandleMarshaller<object>.ConvertToManaged(instancePtr);
    }

    [UnmanagedCallersOnly]
    public static void Free(nint instancePtr)
    {
        GCHandleMarshaller<object>.Free(instancePtr);
    }

    [UnmanagedCallersOnly]
    public static void Invoke(nint instancePtr, UnmanagedNativeString name, IntPtr args, ulong argCount, IntPtr retPtr)
    {
        var instance = Get(instancePtr)!;
        var method = instance.GetType().GetMethod(name!) ?? throw new MissingMethodException();

        var arguments = Marshalling.PtrToParams(method, args, argCount);
        var ret = method.Invoke(instance, arguments);
        if (retPtr != IntPtr.Zero)
        {
            if (ret is null || ret.GetType() == typeof(void))
            {
                Marshal.WriteIntPtr(retPtr, nint.Zero);
            }
            else
            {
                Marshalling.StructureToPtrEx(ret, retPtr);
            }
        }
    }

    [UnmanagedCallersOnly]
    public static void GetField(nint instancePtr, UnmanagedNativeString name, IntPtr retPtr)
    {
        var instance = Get(instancePtr)!;
        var field = instance.GetType().GetField(name!) ?? throw new MissingFieldException();
        var value = field.GetValue(instance);
        Marshalling.StructureToPtrEx(value, retPtr);
    }

    [UnmanagedCallersOnly]
    public static void SetField(nint instancePtr, UnmanagedNativeString name, nint valuePtr)
    {
        var instance = Get(instancePtr)!;
        var field = instance.GetType().GetField(name!) ?? throw new MissingFieldException();
        var value = Marshalling.PtrToStructureEx(valuePtr, field.FieldType);
        field.SetValue(instance, value);
    }

    [UnmanagedCallersOnly]
    public static void GetProperty(nint instancePtr, UnmanagedNativeString name, IntPtr retPtr)
    {
        var instance = Get(instancePtr)!;
        var property = instance.GetType().GetProperty(name!) ?? throw new MissingFieldException();
        var value = property.GetValue(instance);
        Marshalling.StructureToPtrEx(value, retPtr);
    }

    [UnmanagedCallersOnly]
    public static void SetProperty(nint instancePtr, UnmanagedNativeString name, nint valuePtr)
    {
        var instance = Get(instancePtr)!;
        var property = instance.GetType().GetProperty(name!) ?? throw new MissingFieldException();
        var value = Marshalling.PtrToStructureEx(valuePtr, property.PropertyType);
        property.SetValue(instance, value);
    }
}
