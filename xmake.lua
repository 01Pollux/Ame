-- defines:
-- AME_NO_IMGUI: disable imgui

--

add_rules("mode.debug", "mode.releasedbg", "mode.check")
set_languages("cxxlatest")

--

includes("Project/Utils.lua")

--

_script_root_dir = os.scriptdir()
_debug_packages = false
_with_symbols = false
_use_exception = false
_vc_runtime = ""

--

add_defines("NOMINMAX")
set_warnings("allextra", "error")

--

set_exceptions("cxx")
_use_exception = true

if is_mode("debug") then
    add_defines("AME_DEBUG")

    set_symbols("debug", "edit")
    set_runtimes("MDd")

    _debug_packages = true
    _with_symbols = true
elseif is_mode("check") then 
    add_defines("AME_DEBUG")
    add_defines("AME_DEBUG_SANITIZE")

    set_symbols("debug")
    set_runtimes("MDd")

    _debug_packages = true
    _with_symbols = true
elseif is_mode("releasedbg") then
    add_defines("AME_RELEASE")

    set_symbols("debug")
    set_runtimes("MD")

    _with_symbols = true
elseif is_mode("release") then
    add_defines("AME_DIST")

    set_symbols("hidden")
    set_runtimes("MD")
end
--

if is_plat("windows") then
    add_defines("AME_PLATFORM_WINDOWS")
elseif is_plat("linux") then
    add_defines("AME_PLATFORM_LINUX")
end
-- comprimise until a fix for https://github.com/SanderMertens/flecs/issues/1032 is found
-- flecs component's id are static and can't be used in multiple dlls
-- add_defines("FLECS_CPP_NO_AUTO_REGISTRATION") 

--

-- linking for asan, adding directory of clang_rt.asan
if is_mode("check") then
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
end
--

local clang_format = file_utils:path_from_root(".clang-format")
local clang_tidy = file_utils:path_from_root(".clang-tidy")

add_extrafiles(clang_format)
add_extrafiles(clang_tidy)

--

includes("Project/Ame.lua")
