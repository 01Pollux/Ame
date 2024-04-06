includes("Deps/Externals/all.lua")

function ame_header_library(group, name, kind, path)
    set_group(group)
    set_kind(kind)
    add_files(path_from_root(path .. "/**.cpp"))
    add_headerfiles(path_from_root(path .. "/**.hpp"))
    add_includedirs(path_from_root(path .. "/Private"), {public = false})
    add_includedirs(path_from_root(path .. "/Public"), {public = true})
    add_filegroups("", {rootdir = "../" .. path .. "/"})
end

function ame_header_executable(group, name, path)
    ame_header_library(group, name, "binary", path)
    add_deps("Engine", {public = true})
end

includes("Core.lua")
includes("Samples.lua")