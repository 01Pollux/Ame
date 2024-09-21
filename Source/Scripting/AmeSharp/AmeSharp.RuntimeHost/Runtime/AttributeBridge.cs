using AmeSharp.Core.Base.Types;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.RuntimeHost.Runtime;

public unsafe partial class AttributeBridge
{
    public static nint Create(Attribute? attribute)
    {
        return attribute is null ? nint.Zero : GCHandleMarshaller<Attribute>.ConvertToUnmanaged(attribute);
    }
    public static Attribute? Get(nint attributePtr)
    {
        return GCHandleMarshaller<Attribute>.ConvertToManaged(attributePtr);
    }

    [UnmanagedCallersOnly]
    public static void Free(nint attributePtr)
    {
        GCHandleMarshaller<Attribute>.Free(attributePtr);
    }

    [UnmanagedCallersOnly]
    public static void GetValue(nint attributePtr, UnmanagedNativeString name, nint valuePtr)
    {
        var attribute = Get(attributePtr)!;
        var value = attribute.GetType().GetField(name!, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance)!.GetValue(attribute);
        Marshalling.StructureToPtrEx(value, valuePtr);
    }
}
