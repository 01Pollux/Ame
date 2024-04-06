add_rules("mode.debug", "mode.release")
set_languages("c++latest")

script_root_dir = os.scriptdir()
function path_from_root(path)
    return script_root_dir .. "/" .. path
end

if is_plat("windows") then
    add_defines("AME_PLATFORM_WINDOWS")
elseif is_plat("linux") then
    add_defines("AME_PLATFORM_LINUX")
end

includes("Project/Ame.lua")