includes("glfw.lua")
includes("diligent_core.lua")
includes("imgui.lua")
includes("concurrencpp.lua")
includes("kangaru.lua")
includes("mimalloc.lua")

add_requires("boost",                       {system = false, debug = _debug_packages, configs = {
    vs_runtime = vs_runtime,
    iostreams = true,
    lzma = true,
    zlib = true,
    bzip2 = true,
    filesystem = true,
    system = true,
    program_options = true,
    serialization = true
}})
add_requires("assimp",                      {system = false, debug = _debug_packages, configs = {asan = _use_asan}})
add_requires("cereal",                      {system = false, debug = _debug_packages})
add_requires("boost_ut",                    {system = false, debug = _debug_packages, configs = {shared = true}})
add_requires("vulkansdk",                   {debug = _debug_packages})
add_requires("ame.mimalloc",                {system = false, debug = _debug_packages})
add_requires("ame.kangaru",                 {system = false, debug = _debug_packages})
add_requires("spdlog",                      {system = false, debug = _debug_packages})
add_requires("fmt",                         {system = false, debug = _debug_packages})
add_requires("flecs v4.0.0",                {system = false, debug = _debug_packages, configs = {symbols = _with_symbols}})
add_requires("freeimage",                   {system = false, debug = _debug_packages, configs = {symbols = _with_symbols}})
add_requires("cryptopp",                    {system = false, debug = _debug_packages, configs = {symbols = _with_symbols}})
add_requires("magic_enum",                  {system = false, debug = _debug_packages})
add_requires("directxmath",                 {system = false, debug = _debug_packages, configs = {symbols = _with_symbols}})
add_requires("ame.concurrencpp",            {system = false, debug = _debug_packages, configs = {symbols = _with_symbols}})
add_requires("ame.glfw",                    {system = false, debug = _debug_packages, configs = {symbols = _with_symbols}})
add_requires("ame.diligent_core",           {system = false, debug = _debug_packages, configs = {symbols = _with_symbols, exceptions = _use_exception}})
add_requires("directxshadercompiler",       {system = false, debug = _debug_packages, configs = {symbols = _with_symbols}})
add_requires("ame.imgui v1.90.9-docking",   {system = false, debug = _debug_packages, configs = {symbols = _width_symbols, freetype = true}})