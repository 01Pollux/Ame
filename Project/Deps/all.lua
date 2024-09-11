includes("glfw.lua")
includes("diligent_core.lua")
includes("imgui.lua")
includes("concurrencpp.lua")
includes("kangaru.lua")
includes("mimalloc.lua")
includes("octree.lua")

local _package_options = function (opts) 
    opts = opts or {}
    opts["system"] = opts["system"] or false
    opts["debug"] = opts["debug"] or _debug_packages
    opts["configs"] = opts["configs"] or {}
    opts["configs"]["symbols"] = opts["configs"]["symbols"] or _with_symbols
    opts["configs"]["exceptions"] = opts["configs"]["exceptions"] or _use_exception

    return opts
end

add_requires("boost 1.86.0",                _package_options({configs = {
    iostreams = true,
    lzma = true,
    zlib = true,
    bzip2 = true,
    filesystem = true,
    system = true,
    stacktrace = true,
    program_options = true,
    serialization = true,
}}))
add_requires("assimp",                      _package_options())
add_requires("cereal",                      _package_options())
add_requires("boost_ut",                    _package_options({configs = {shared = true}}))
add_requires("vulkansdk")
add_requires("ame.mimalloc",                _package_options({configs = {shared = true}}))
add_requires("ame.kangaru",                 _package_options())
add_requires("spdlog",                      _package_options())
add_requires("fmt",                         _package_options())
add_requires("flecs v4.0.1",                _package_options({configs = {shared = true}}))
add_requires("freeimage",                   _package_options({configs = {rgb = true}}))
add_requires("cryptopp",                    _package_options())
add_requires("magic_enum",                  _package_options())
add_requires("directxmath",                 _package_options())
add_requires("ame.concurrencpp",            _package_options())
add_requires("ame.glfw",                    _package_options())
add_requires("ame.diligent_core",           _package_options())
add_requires("directxshadercompiler",       _package_options())
add_requires("ame.imgui v1.91.1-docking",   _package_options({configs = {shared = true, freetype = true}}))
add_requires("ame.octree",                  _package_options())
