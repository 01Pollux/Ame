﻿using AmeSharp.Core.Base;
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
    }
}
