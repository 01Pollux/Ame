using AmeSharp.Core.Base.Types;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Runtime;

public unsafe partial class FieldBridge
{
    public static nint Create(FieldInfo? field)
    {
        return field is null ? nint.Zero : GCHandleMarshaller<FieldInfo>.ConvertToUnmanaged(field);
    }
    public static FieldInfo? Get(nint fieldPtr)
    {
        return GCHandleMarshaller<FieldInfo>.ConvertToManaged(fieldPtr) as FieldInfo;
    }

    [UnmanagedCallersOnly]
    public static void Free(nint fieldPtr)
    {
        GCHandleMarshaller<FieldInfo>.Free(fieldPtr);
    }

    [UnmanagedCallersOnly]
    public static UnmanagedNativeString GetName(nint fieldPtr)
    {
        var field = Get(fieldPtr);
        return field?.Name ?? string.Empty;
    }

    [UnmanagedCallersOnly]
    public static bool IsStatic(nint fieldPtr)
    {
        var field = Get(fieldPtr);
        return field?.IsStatic ?? false;
    }

    [UnmanagedCallersOnly]
    public static void GetValue(nint fieldPtr, nint instancePtr, IntPtr retPtr)
    {
        var field = Get(fieldPtr);
        var instance = InstanceBridge.Get(instancePtr);
        var value = field!.GetValue(instance);
        Marshalling.StructureToPtrEx(value, retPtr);
    }

    [UnmanagedCallersOnly]
    public static void SetValue(nint fieldPtr, nint instancePtr, nint valuePtr)
    {
        var field = Get(fieldPtr);
        var instance = InstanceBridge.Get(instancePtr);
        var value = Marshalling.PtrToStructureEx(valuePtr, field!.FieldType);
        field.SetValue(instance, value);
    }
}
