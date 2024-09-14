using System.Reflection;
using System.Runtime.Loader;

namespace AmeSharp.Bridge.Core.Runtime
{
    internal class ALCWrapper(AssemblyLoadContext context)
    {
        private AssemblyLoadContext _context = context;
        private List<nint> _assemblies = [];

        public void UnloadContext()
        {
            _assemblies.ForEach(GCHandleMarshaller<Assembly>.Free);
            _context.Unload();
        }

        public nint LoadAssembly(string path)
        {
            Assembly assembly = _context.LoadFromAssemblyPath(path);
            nint library = AssemblyBridge.Create(assembly);
            _assemblies.Add(library);
            return library;
        }
        public nint LoadAssembly(Stream stream)
        {
            Assembly assembly = _context.LoadFromStream(stream);
            nint library = AssemblyBridge.Create(assembly);
            _assemblies.Add(library);
            return library;
        }
    }
}
