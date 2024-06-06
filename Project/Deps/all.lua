includes("glfw.lua")
includes("nri.lua")
includes("concurrencpp.lua")
includes("kangaru.lua")

add_requires("boost_ut", {configs = {shared = true}})
add_requires("boost", {configs = {
    filesystem = true,
    system = true,
    program_options = true,
    serialization = true
}})
add_requires("mimalloc")
add_requires("ame.kangaru")
add_requires("spdlog")
add_requires("fmt")
add_requires("flecs")
add_requires("freeimage")
add_requires("cryptopp")
add_requires("magic_enum")
add_requires("glm")
add_requires("ame.concurrencpp")
add_requires("ame.glfw")
add_requires("ame.nri")
add_requires("directxshadercompiler")