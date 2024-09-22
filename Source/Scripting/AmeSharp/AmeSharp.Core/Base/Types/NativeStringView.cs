using AmeSharp.Core.Marshallers.Base;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Base.Types;

[NativeMarshalling(typeof(NativeStringViewMarshaller))]
public record NativeStringView(string Value)
{
    public static implicit operator NativeStringView(string value) => new(value);
    public static implicit operator string(NativeStringView value) => value.Value;
}
