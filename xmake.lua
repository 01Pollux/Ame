includes("Project/Rules.lua")
add_rules("mode.Debug", "mode.Release", "mode.Dist")
set_languages("c++latest")

_script_root_dir = os.scriptdir()

if is_plat("windows") then
    add_defines("AME_PLATFORM_WINDOWS")
elseif is_plat("linux") then
    add_defines("AME_PLATFORM_LINUX")
end

add_defines("NOMINMAX")

includes("Project/Ame.lua")