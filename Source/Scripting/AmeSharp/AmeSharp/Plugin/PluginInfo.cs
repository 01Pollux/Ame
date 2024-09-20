using AmeSharp.Core.Base.Types;
using AmeSharp.Plugin.Marshallers;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Plugin;

[NativeMarshalling(typeof(PluginInfoMarshaller))]
public record PluginInfo(string Name, string Author, string Description, DateTime Date, VersionT Version)
{
}
