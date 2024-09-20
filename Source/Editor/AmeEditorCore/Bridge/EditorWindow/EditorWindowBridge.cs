using AmeSharp.Core.Base.Marshallers;
using AmeSharp.Core.Base.Types;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace AmeEditorCore.Bridge.EditorWindow;

internal unsafe partial class EditorWindowBridge
{
    [LibraryImport(Libraries.AmeEditorHost, EntryPoint = "Ame_EditorWindow_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create(NativeStringView path);

    [LibraryImport(Libraries.AmeEditorHost, EntryPoint = "Ame_EditorWindow_SetOnDrawVisible")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetOnDrawVisible(IntPtr editorWindowObject, delegate* unmanaged[Cdecl]<IntPtr, void> callback);

    [LibraryImport(Libraries.AmeEditorHost, EntryPoint = "Ame_EditorWindow_SetOnToolbarDraw")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetOnToolbalDraw(IntPtr editorWindowObject, delegate* unmanaged[Cdecl]<IntPtr, void> callback);

    [LibraryImport(Libraries.AmeEditorHost, EntryPoint = "Ame_EditorWindow_SetOnShow")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetOnShow(IntPtr editorWindowObject, delegate* unmanaged[Cdecl]<IntPtr, void> callback);

    [LibraryImport(Libraries.AmeEditorHost, EntryPoint = "Ame_EditorWindow_SetOnHide")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetOnHide(IntPtr editorWindowObject, delegate* unmanaged[Cdecl]<IntPtr, void> callback);

    [LibraryImport(Libraries.AmeEditorHost, EntryPoint = "Ame_EditorWindow_GetFullPath")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial NativeStringView GetFullPath(IntPtr editorWindowObject);
}
