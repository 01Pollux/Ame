includes("Deps/Externals/all.lua")
includes("Utils.lua")

set_languages("cxxlatest")

local clang_format = file_utils:path_from_root(".clang-format")
add_extrafiles(clang_format)

includes("Core.lua")
includes("Samples.lua")