using System.Reflection;
using System.Runtime.Loader;

namespace AmeSharp.Bridge.Core.Runtime
{
    internal class ALCWrapper
    {
        private AssemblyLoadContext _context ;
        private List<nint> _assemblies = [];

        public ALCWrapper(AssemblyLoadContext context)
        {
            _context = context;
            context.Resolving +=  (_, name) =>
            {
                return (from curContext in AssemblyLoadContext.All
                        from curAsm in curContext.Assemblies
                        where curAsm.GetName().FullName == name.FullName
                        select curAsm).FirstOrDefault();
            };
        }

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
