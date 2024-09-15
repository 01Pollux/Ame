﻿using AmeSharp.Core.Base;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Runtime
{
    public unsafe partial class AttributeBridge
    {
        public static nint Create(Attribute? attribute)
        {
            return attribute is null ? nint.Zero : GCHandleMarshaller<Attribute>.ConvertToUnmanaged(attribute);
        }

        [UnmanagedCallersOnly]
        public static void Free(nint attributePtr)
        {
            GCHandleMarshaller<Attribute>.Free(attributePtr);
        }

        [UnmanagedCallersOnly]
        public static nint GetType(nint attributePtr)
        {
            var attribute = GCHandleMarshaller<Attribute>.ConvertToManaged(attributePtr)!;
            return TypeBridge.Create(attribute.GetType());
        }

        [UnmanagedCallersOnly]
        public static void GetValue(nint attributePtr, UnmanagedNativeString name, IntPtr valuePtr)
        {
            var attribute = GCHandleMarshaller<Attribute>.ConvertToManaged(attributePtr)!;
            var value = attribute.GetType().GetField(name!, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance)!.GetValue(attribute);
            Marshalling.StructureToPtrEx(value, valuePtr);
        }
    }
}
