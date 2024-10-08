﻿using AmeSharp.Core.Base.Types;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.RuntimeHost.Runtime;

internal static class Marshalling
{
    public static object? PtrArrayToStructureEx(nint ptr, Type? type)
    {
        if (type is null)
        {
            return null;
        }

        var storage = Marshal.PtrToStructure<RawUnmanagedNativeArray>(ptr);
        if (type.IsGenericType) // Generic type
        {
            var genericType = type.GetGenericTypeDefinition();
            if (genericType == typeof(UnmanagedNativeArray<>))
            {
                type = typeof(UnmanagedNativeArray<>).MakeGenericType(type.GetGenericArguments());
                return Activator.CreateInstance(type, storage);
            }
        }
        if (type.IsSZArray)
        {
            var value = Array.CreateInstance(type, (int)storage.Length);
            for (int i = 0; i < (int)storage.Length; i++)
            {
                var elementPtr = storage.Data + i * Marshal.SizeOf<RawUnmanagedNativeArray>();
                var subStorage = Marshal.PtrToStructure<RawUnmanagedNativeArray>(elementPtr);
                unsafe
                {
                    var subStoragePtr = (nint)Unsafe.AsPointer(ref subStorage);
                    value.SetValue(PtrArrayToStructureEx(subStoragePtr, type.GetElementType()), i);
                }
            }
            return value;
        }
        else
        {
            var value = Array.CreateInstance(type, (int)storage.Length);
            int elementSize = type == typeof(string) ? Marshal.SizeOf<UnmanagedNativeString>() : Marshal.SizeOf(type);

            for (int i = 0; i < (int)storage.Length; i++)
            {
                var elementPtr = storage.Data + i * elementSize;
                value.SetValue(PtrToStructureEx(elementPtr, type), i);
            }

            return value;
        }
    }

    public static object? PtrToStructureEx(nint ptr, Type type)
    {
        if (type == typeof(string))
        {
            return (string?)Marshal.PtrToStructure<UnmanagedNativeString>(ptr);
        }
        if (type.IsGenericType) // Generic type
        {
            var genericType = type.GetGenericTypeDefinition();
            if (genericType == typeof(UnmanagedNativeArray<>))
            {
                var storage = Marshal.PtrToStructure<RawUnmanagedNativeArray>(ptr);
                type = typeof(UnmanagedNativeArray<>).MakeGenericType(type.GetGenericArguments());
                return Activator.CreateInstance(type, storage);
            }
        }
        else if (type.IsSZArray)
        {
            return PtrArrayToStructureEx(ptr, type.GetElementType());
        }
        return Marshal.PtrToStructure(ptr, type);
    }

    public static void StructureToPtrEx(object? value, nint ptr)
    {
        if (value is null)
        {
            return;
        }
        if (value is string str)
        {
            Marshal.StructureToPtr(new UnmanagedNativeString(str), ptr, false);
        }
        else if (value is Array array)
        {
            var storage = Marshal.PtrToStructure<RawUnmanagedNativeArray>(ptr);
            int elementSize = 0;
            if (array.GetType().GetElementType() == typeof(string))
            {
                elementSize = Marshal.SizeOf<UnmanagedNativeString>();
            }
            else
            {
                var type = array.GetType().GetElementType();
                if (type is null)
                {
                    return;
                }
                if (type.IsGenericType)
                {
                    var genericType = type.GetGenericTypeDefinition();
                    if (genericType == typeof(UnmanagedNativeArray<>))
                    {
                        elementSize = Marshal.SizeOf<RawUnmanagedNativeArray>();
                    }
                    else
                    {
                        elementSize = Marshal.SizeOf(type);
                    }
                }
                else if (type.IsSZArray)
                {
                    elementSize = Marshal.SizeOf<RawUnmanagedNativeArray>();
                }
                else
                {
                    elementSize = Marshal.SizeOf(type);
                }
            }
            for (int i = 0; i < array.Length; i++)
            {
                var elementPtr = storage.Data + i * elementSize;
                StructureToPtrEx(array.GetValue(i), elementPtr);
            }
        }
        else
        {
            Marshal.StructureToPtr(value, ptr, false);
        }
    }

    public static object?[]? PtrToParams(MethodBase? method, nint args, ulong argCount)
    {
        if (method is null || args == nint.Zero)
        {
            return null;
        }

        var paramInfos = method.GetParameters();
        var result = new object?[paramInfos.Length];
        Debug.Assert(paramInfos.Length >= (int)argCount, "Parameter count mismatch");

        for (int i = 0; i < paramInfos.Length; i++)
        {
            nint valPtr = nint.Zero;

            bool useDefault = (ulong)i >= argCount;
            if (!useDefault)
            {
                valPtr = Marshal.ReadIntPtr(args + i * Marshal.SizeOf<nint>());
                useDefault = valPtr == nint.Zero;
            }
            // if param has default argument, it will be null
            if (useDefault)
            {
                result[i] = paramInfos[i].HasDefaultValue ? paramInfos[i].DefaultValue : null;
            }
            else
            {
                var paramType = paramInfos[i].ParameterType;
                result[i] = PtrToStructureEx(valPtr, paramType);
            }
        }
        return result;
    }

    public static UnmanagedNativeArray<nint> ParamsToPtr(MethodInfo? method)
    {
        if (method is null)
        {
            return new();
        }
        var paramInfos = method.GetParameters();
        UnmanagedNativeArray<nint> result = new((ulong)paramInfos.Length);
        for (int i = 0; i < paramInfos.Length; i++)
        {
            var handle = TypeBridge.Create(paramInfos[i].ParameterType);
            Marshal.WriteIntPtr(result.Data, i * Marshal.SizeOf<nint>(), handle);
        }
        return result;
    }
}
