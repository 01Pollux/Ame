includes("Project/Utils.lua")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
set_languages("cxxlatest")

--

_script_root_dir = os.scriptdir()
_use_asan = false
_use_exception = false
_debug_packages = false
_vc_runtime = ""

--

if is_mode("debug") then
    _vc_runtime = "MTd"
else 
    _vc_runtime = "MT"
end

--

set_runtimes(_vc_runtime)
add_defines("NOMINMAX")

--

--

if is_mode("debug") then
    add_defines("DEBUG")
    add_defines("AME_DEBUG")
    add_defines("AME_ASSET_MGR_DISABLE_HASH_VALIDATION")

    set_policy("build.sanitizer.address", true)
    set_exceptions("cxx", "objc")

    _use_asan = true
    _use_exception = true
    _debug_packages = true
end

if is_mode("releasedbg") then
    add_defines("DNDEBUG")
    add_defines("AME_ASSET_MGR_DISABLE_HASH_VALIDATION")
    add_defines("AME_RELEASE")
end

if is_mode("release") then
    add_defines("DNDEBUG")
    add_defines("AME_ASSET_MGR_DISABLE_HASH_VALIDATION")
    add_defines("AME_DIST")
end

--

if is_plat("windows") then
    add_defines("AME_PLATFORM_WINDOWS")
elseif is_plat("linux") then
    add_defines("AME_PLATFORM_LINUX")
end
add_defines("FLECS_CPP_NO_AUTO_REGISTRATION")

--

local clang_format = file_utils:path_from_root(".clang-format")
local clang_tidy = file_utils:path_from_root(".clang-tidy")

add_extrafiles(clang_format)
add_extrafiles(clang_tidy)

--

includes("Project/Ame.lua")