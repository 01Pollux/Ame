includes("glfw.lua")
includes("nri.lua")
includes("concurrencpp.lua")
includes("kangaru.lua")
includes("mimalloc.lua")
includes("octree.lua")

add_requires("boost", {system = false, debug = _debug_packages, configs = {
    vs_runtime = vs_runtime,
    filesystem = true,
    system = true,
    program_options = true,
    serialization = true
    }})
add_requires("boost_ut",                {system = false, debug = _debug_packages, configs = {shared = true}})
add_requires("ame.mimalloc",            {system = false, debug = _debug_packages})
add_requires("ame.kangaru",             {system = false, debug = _debug_packages})
add_requires("spdlog",                  {system = false, debug = _debug_packages})
add_requires("fmt",                     {system = false, debug = _debug_packages})
add_requires("flecs",                   {system = false, debug = _debug_packages})
add_requires("freeimage",               {system = false, debug = _debug_packages})
add_requires("cryptopp",                {system = false, debug = _debug_packages})
add_requires("magic_enum",              {system = false, debug = _debug_packages})
add_requires("directxmath",             {system = false, debug = _debug_packages})
add_requires("ame.concurrencpp",        {system = false, debug = _debug_packages})
add_requires("ame.glfw",                {system = false, debug = _debug_packages})
add_requires("ame.nri",                 {system = false, debug = _debug_packages})
add_requires("directxshadercompiler",   {system = false, debug = _debug_packages})
add_requires("ame.octree",              {system = false, debug = _debug_packages})