includes("Deps/Externals/all.lua")

function ame_header_library(group, name, kind, path)
    set_group(group)
    set_kind(kind)
    
    local source_files = path_from_root(path .. "/**.cpp")
    if table.empty(source_files) ~= true then
        add_files(source_files)
    end

    local header_files = path_from_root(path .. "/**.hpp")
    if table.empty(header_files) ~= true then
        add_headerfiles(header_files)
    end

    local include_dirs = path_from_root(path .. "/Public")
    local folder_exists = os.isdir(include_dirs)
    if folder_exists == true then
        add_includedirs(include_dirs, {public = true})
    end

    include_dirs = path_from_root(path .. "/Private")
    folder_exists = os.isdir(include_dirs)
    if folder_exists == true then
        add_includedirs(include_dirs, {public = false})
    end

    add_filegroups("", {rootdir = "../" .. path .. "/"})
end

function ame_header_executable(group, name, path)
    ame_header_library(group, name, "binary", path)
    add_deps("Engine", {public = true})
end

-- 

includes("Core.lua")
includes("Samples.lua")