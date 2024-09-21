using AmeSharp.Core.Base.Types;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.RuntimeHost.Runtime;

public unsafe partial class TypeBridge
{
    public static nint Create(Type? type)
    {
        return type is null ? nint.Zero : GCHandleMarshaller<Type>.ConvertToUnmanaged(type);
    }
    public static Type? Get(nint typePtr)
    {
        return GCHandleMarshaller<Type>.ConvertToManaged(typePtr);
    }

    [UnmanagedCallersOnly]
    public static void Free(nint typePtr)
    {
        GCHandleMarshaller<Type>.Free(typePtr);
    }

    [UnmanagedCallersOnly]
    public static UnmanagedNativeString GetName(nint typePtr)
    {
        return Get(typePtr)!.Name;
    }

    [UnmanagedCallersOnly]
    public static nint GetBaseType(nint typePtr)
    {
        var type = Get(typePtr)!;
        return Create(type.BaseType);
    }

    [UnmanagedCallersOnly]
    public static bool CastAs(nint typePtr, nint otherTypePtr)
    {
        var type = Get(typePtr)!;
        var otherType = Get(otherTypePtr)!;
        return otherType.IsAssignableFrom(type);
    }

    [UnmanagedCallersOnly]
    public static ulong GetSize(nint typePtr)
    {
        var type = Get(typePtr)!;
        try
        {
            return (ulong)Marshal.SizeOf(type);
        }
        catch
        {
            return 0;
        }
    }

    [UnmanagedCallersOnly]
    public static nint CreateInstance(nint typePtr, nint args, ulong argCount)
    {
        var type = Get(typePtr)!;
        var ctor = type.GetConstructors(BindingFlags.Public | BindingFlags.Instance).FirstOrDefault();
        var arguments = Marshalling.PtrToParams(ctor, args, argCount);
        if (arguments is null)
        {
            return nint.Zero;
        }
        return InstanceBridge.Create(ctor?.Invoke(arguments));
    }

    //

    [UnmanagedCallersOnly]
    public static RawUnmanagedNativeArray GetMethods(nint typePtr)
    {
        var type = Get(typePtr)!;
        var methods = type.GetMethods();
        return new UnmanagedNativeArray<nint>(methods.Select(MethodBridge.Create)).Storage;
    }
    [UnmanagedCallersOnly]
    public static nint GetMethod(nint typePtr, UnmanagedNativeString name)
    {
        var type = Get(typePtr)!;
        var method = type.GetMethod(name!);
        return MethodBridge.Create(method);
    }

    //

    [UnmanagedCallersOnly]
    public static RawUnmanagedNativeArray GetAttributes(nint typePtr)
    {
        var type = Get(typePtr)!;
        var attributes = type.GetCustomAttributes();
        return new UnmanagedNativeArray<nint>(attributes.Select(AttributeBridge.Create)).Storage;
    }
    [UnmanagedCallersOnly]
    public static nint GetAttribute(nint typePtr, UnmanagedNativeString name)
    {
        string nameStr = name!;
        var type = Get(typePtr)!;
        var attribute = type.GetCustomAttributes().FirstOrDefault(x => x.GetType().FullName == nameStr);
        return AttributeBridge.Create(attribute);
    }

    //

    [UnmanagedCallersOnly]
    public static RawUnmanagedNativeArray GetFields(nint typePtr)
    {
        var type = Get(typePtr)!;
        var fields = type.GetFields();
        return new UnmanagedNativeArray<nint>(fields.Select(FieldBridge.Create)).Storage;
    }
    [UnmanagedCallersOnly]
    public static nint GetField(nint typePtr, UnmanagedNativeString name)
    {
        var type = Get(typePtr)!;
        var field = type.GetField(name!);
        return FieldBridge.Create(field);
    }

    //

    [UnmanagedCallersOnly]
    public static RawUnmanagedNativeArray GetProperties(nint typePtr)
    {
        var type = Get(typePtr)!;
        var properties = type.GetProperties();
        return new UnmanagedNativeArray<nint>(properties.Select(PropertyBridge.Create)).Storage;
    }
    [UnmanagedCallersOnly]
    public static nint GetProperty(nint typePtr, UnmanagedNativeString name)
    {
        var type = Get(typePtr)!;
        var property = type.GetProperty(name!);
        return PropertyBridge.Create(property);
    }
}
