﻿using AmeSharp.Core.Base;
using System.Reflection;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Runtime
{
    public unsafe partial class MethodBridge
    {
        public static nint Create(MethodInfo? method)
        {
            return method is null ? nint.Zero : GCHandleMarshaller<MethodInfo>.ConvertToUnmanaged(method);
        }

        [UnmanagedCallersOnly]
        public static void Free(nint methodPtr)
        {
            GCHandleMarshaller<MethodInfo>.Free(methodPtr);
        }

        [UnmanagedCallersOnly]
        public static bool IsStatic(nint methodPtr)
        {
            var method = GCHandleMarshaller<MethodInfo>.ConvertToManaged(methodPtr);
            return method?.IsStatic ?? false;
        }

        [UnmanagedCallersOnly]
        public static UnmanagedNativeString GetName(nint methodPtr)
        {
            var method = GCHandleMarshaller<MethodInfo>.ConvertToManaged(methodPtr);
            return method?.Name ?? string.Empty;
        }

        [UnmanagedCallersOnly]
        public static RawUnmanagedNativeArray GetParamTypes(nint methodPtr)
        {
            var method = GCHandleMarshaller<MethodInfo>.ConvertToManaged(methodPtr);
            return Marshalling.ParamsToPtr(method).Storage;
        }

        [UnmanagedCallersOnly]
        public static nint GetReturnType(nint methodPtr)
        {
            var method = GCHandleMarshaller<MethodInfo>.ConvertToManaged(methodPtr);
            return TypeBridge.Create(method?.ReturnType);
        }

        [UnmanagedCallersOnly]
        public static void Invoke(nint methodPtr, nint obj, IntPtr args, ulong argCount, IntPtr retPtr)
        {
            var method = GCHandleMarshaller<MethodInfo>.ConvertToManaged(methodPtr)!;
            var arguments = Marshalling.PtrToParams(method, args, argCount);

            object? thisPtr = obj == nint.Zero ? null : GCHandleMarshaller<object>.ConvertToManaged(obj);
            var ret = method.Invoke(thisPtr, arguments);
            if (retPtr != IntPtr.Zero)
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
}