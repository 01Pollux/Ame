using AmeSharp.Core.Base.Marshallers;
using AmeSharp.Core.Base.Types;
using AmeSharp.Core.Plugin;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Plugin.Marshallers;

[CustomMarshaller(typeof(PluginInfo), MarshalMode.Default, typeof(PluginInfoMarshaller))]
internal static unsafe class PluginInfoMarshaller
{
    internal unsafe struct Unmanaged
    {
        public byte* Name;
        public byte* Author;
        public byte* Description;
        public byte* Date;
        public VersionT Version;
    }

    public static Unmanaged ConvertToUnmanaged(PluginInfo managed)
    {
        return new Unmanaged
        {
            Name = Utf8StringMarshaller.ConvertToUnmanaged(managed.Name),
            Author = Utf8StringMarshaller.ConvertToUnmanaged(managed.Author),
            Description = Utf8StringMarshaller.ConvertToUnmanaged(managed.Description),
            Date = Utf8StringMarshaller.ConvertToUnmanaged(managed.Date.ToString("yyyy-MM-dd HH:mm:ss")),
            Version = managed.Version
        };
    }

    public static PluginInfo ConvertToManaged(Unmanaged unmanaged)
    {
        return new(
            Name: Utf8StringMarshaller.ConvertToManaged(unmanaged.Name) ?? string.Empty,
            Author: Utf8StringMarshaller.ConvertToManaged(unmanaged.Author) ?? string.Empty,
            Description: Utf8StringMarshaller.ConvertToManaged(unmanaged.Description) ?? string.Empty,
            Date: DateTime.Parse(Utf8StringMarshaller.ConvertToManaged(unmanaged.Date) ?? string.Empty),
            Version: unmanaged.Version
        );
    }

    public static void Free(Unmanaged unmanaged)
    {
        Utf8StringMarshaller.Free(unmanaged.Name);
        Utf8StringMarshaller.Free(unmanaged.Author);
        Utf8StringMarshaller.Free(unmanaged.Description);
        Utf8StringMarshaller.Free(unmanaged.Date);
    }
}
