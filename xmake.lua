includes("Project/Rules.lua")
includes("Project/Utils.lua")

add_rules("mode.Debug", "mode.Release", "mode.Dist")
set_languages("c++latest")

_script_root_dir = os.scriptdir()

if is_plat("windows") then
    add_defines("AME_PLATFORM_WINDOWS")
elseif is_plat("linux") then
    add_defines("AME_PLATFORM_LINUX")
end

add_defines("NOMINMAX")

local clang_format = file_utils:path_from_root(".clang-format")
local clang_tidy = file_utils:path_from_root(".clang-tidy")

add_extrafiles(clang_format)
add_extrafiles(clang_tidy)

includes("Project/Ame.lua")