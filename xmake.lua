includes("Project/Utils.lua")

add_rules("mode.debug", "mode.releasedbg")
set_languages("cxxlatest")

--

_script_root_dir = os.scriptdir()
_use_asan = false
_use_exception = false
_debug_packages = false
_width_symbols = false
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
-- linking for asan, adding directory of clang_rt.asan
on_load(function (target)
    local msvc = target:toolchain("msvc")
    if msvc then
        local runenvs = msvc:runenvs()
        if runenvs then
            local lib_path = runenvs["LIB"]
            -- split by ';' lib_path
            for cur_path in lib_path:gmatch("([^;]+)") do
                target:add("linkdirs", cur_path)
            end
        end
    end
end)

if is_mode("debug") then
    add_defines("DEBUG")
    add_defines("AME_DEBUG")
    add_defines("AME_ASSET_MGR_DISABLE_HASH_VALIDATION")

    set_policy("build.sanitizer.address", true)
    set_exceptions("cxx", "objc")

    _use_asan = true
    _use_exception = true
    _debug_packages = true
    _width_symbols = true
end

if is_mode("releasedbg") then
    add_defines("DNDEBUG")
    add_defines("AME_ASSET_MGR_DISABLE_HASH_VALIDATION")
    add_defines("AME_RELEASE")
    _width_symbols = true
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
