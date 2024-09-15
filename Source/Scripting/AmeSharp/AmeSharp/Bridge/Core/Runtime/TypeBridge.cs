using AmeSharp.Core.Base;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Runtime
{
    public unsafe partial class TypeBridge
    {
        public static nint Create(Type? type)
        {
            return type is null ? nint.Zero : GCHandleMarshaller<Type>.ConvertToUnmanaged(type);
        }

        [UnmanagedCallersOnly]
        public static void Free(nint typePtr)
        {
            GCHandleMarshaller<Type>.Free(typePtr);
        }

        [UnmanagedCallersOnly]
        public static UnmanagedNativeString GetName(nint typePtr)
        {
            return GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!.Name;
        }

        [UnmanagedCallersOnly]
        public static nint GetBaseType(nint typePtr)
        {
            var type = GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!;
            return Create(type.BaseType);
        }

        [UnmanagedCallersOnly]
        public static bool CastAs(nint typePtr, nint otherTypePtr)
        {
            var type = GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!;
            var otherType = GCHandleMarshaller<Type>.ConvertToManaged(otherTypePtr)!;
            return otherType.IsAssignableFrom(type);
        }

        [UnmanagedCallersOnly]
        public static ulong GetSize(nint typePtr)
        {
            var type = GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!;
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
        public static nint CreateInstance(nint typePtr, IntPtr args, ulong argCount)
        {
            var type = GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!;
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
            var type = GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!;
            var methods = type.GetMethods();
            return (new UnmanagedNativeArray<nint>(methods.Select(MethodBridge.Create))).Storage;
        }

        [UnmanagedCallersOnly]
        public static nint GetMethod(nint typePtr, UnmanagedNativeString name)
        {
            var type = GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!;
            var method = type.GetMethod(name!);
            return MethodBridge.Create(method);
        }

        //

        [UnmanagedCallersOnly]
        public static RawUnmanagedNativeArray GetAttributes(nint typePtr)
        {
            var type = GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!;
            var attributes = type.GetCustomAttributes();
            return (new UnmanagedNativeArray<nint>(attributes.Select(AttributeBridge.Create))).Storage;
        }

        [UnmanagedCallersOnly]
        public static nint GetAttribute(nint typePtr, UnmanagedNativeString name)
        {
            string nameStr = name!;
            var type = GCHandleMarshaller<Type>.ConvertToManaged(typePtr)!;
            var attribute = type.GetCustomAttributes().FirstOrDefault(x => x.GetType().FullName == nameStr);
            return AttributeBridge.Create(attribute);
        }
    }
}
