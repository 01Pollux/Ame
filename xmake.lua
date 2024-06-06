includes("Project/Rules.lua")
includes("Project/Utils.lua")

add_rules("mode.Debug", "mode.Release", "mode.Dist")
set_languages("c++latest")

--

_script_root_dir = os.scriptdir()
_use_asan = false
_use_exception = false

local clang_format = file_utils:path_from_root(".clang-format")
local clang_tidy = file_utils:path_from_root(".clang-tidy")

add_extrafiles(clang_format)
add_extrafiles(clang_tidy)

--

if is_plat("windows") then
    add_defines("AME_PLATFORM_WINDOWS")
elseif is_plat("linux") then
    add_defines("AME_PLATFORM_LINUX")
end

if is_mode("Debug") then
    -- set_policy("build.sanitizer.address", true)
    -- _use_asan = true
    set_exceptions("cxx", "objc")
    _use_exception = true
end

add_defines("NOMINMAX")

--

includes("Project/Ame.lua")