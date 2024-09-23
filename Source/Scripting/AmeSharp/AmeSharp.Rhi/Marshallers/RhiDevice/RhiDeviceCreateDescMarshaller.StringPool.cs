using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Rhi.Marshallers.RhiDevice;

internal static unsafe partial class RhiDeviceCreateDescMarshaller
{
    public class StringPool : IDisposable
    {
        private List<IntPtr> _strings = [];

        ~StringPool() => DisposeNoGcSupression();

        public byte** Insert(string[] strings)
        {
            if (strings.Length == 0)
            {
                return null;
            }

            var byteSize = Unsafe.SizeOf<IntPtr>() * (strings.Length + 1);
            byte** list = (byte**)NativeMemory.Alloc((nuint)byteSize);
            for (int i = 0; i < strings.Length; i++)
            {
                list[i] = Utf8StringMarshaller.ConvertToUnmanaged(strings[i]);
            }
            list[strings.Length] = null;

            _strings.Add((IntPtr)list);
            return list;
        }

        public void Dispose()
        {
            DisposeNoGcSupression();
            GC.SuppressFinalize(this);
        }

        private void DisposeNoGcSupression()
        {
            foreach (var list in _strings)
            {
                var bytes = (byte**)list.ToPointer();
                var str = bytes;
                while (*str != null)
                {
                    Utf8StringMarshaller.Free(*str);
                    str++;
                }
                NativeMemory.Free(bytes);
            }
        }
    }
}
