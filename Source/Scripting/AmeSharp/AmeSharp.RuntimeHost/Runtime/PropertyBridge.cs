using AmeSharp.Core.Base.Types;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.RuntimeHost.Runtime;

public unsafe partial class PropertyBridge
{
    public static nint Create(PropertyInfo? prop)
    {
        return prop is null ? nint.Zero : GCHandleMarshaller<PropertyInfo>.ConvertToUnmanaged(prop);
    }
    public static PropertyInfo? Get(nint propPtr)
    {
        return GCHandleMarshaller<PropertyInfo>.ConvertToManaged(propPtr) as PropertyInfo;
    }

    [UnmanagedCallersOnly]
    public static void Free(nint propPtr)
    {
        GCHandleMarshaller<PropertyInfo>.Free(propPtr);
    }

    [UnmanagedCallersOnly]
    public static UnmanagedNativeString GetName(nint propPtr)
    {
        var prop = Get(propPtr);
        return prop?.Name ?? string.Empty;
    }

    [UnmanagedCallersOnly]
    public static bool IsStatic(nint propPtr)
    {
        var prop = Get(propPtr);
        return prop?.GetMethod?.IsStatic ?? false;
    }

    [UnmanagedCallersOnly]
    public static void GetValue(nint propPtr, nint instancePtr, nint retPtr)
    {
        var prop = Get(propPtr);
        var instance = InstanceBridge.Get(instancePtr);
        var value = prop!.GetValue(instance);
        Marshalling.StructureToPtrEx(value, retPtr);
    }

    [UnmanagedCallersOnly]
    public static void SetValue(nint propPtr, nint instancePtr, nint valuePtr)
    {
        var prop = Get(propPtr);
        var instance = InstanceBridge.Get(instancePtr);
        var value = Marshalling.PtrToStructureEx(valuePtr, prop!.PropertyType);
        prop.SetValue(instance, value);
    }
}
