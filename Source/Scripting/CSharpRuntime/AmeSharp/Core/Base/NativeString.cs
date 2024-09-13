using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Base
{
    [NativeMarshalling(typeof(NativeStringMarshaller))]
    public record NativeString(string Value)
    {
        public static implicit operator NativeString(string value) => new(value);
        public static implicit operator string(NativeString value) => value.Value;
        public override string ToString() => Value;

        public static NativeString Empty { get; } = new(string.Empty);
    }
}
