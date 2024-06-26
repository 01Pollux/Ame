ame_utils = {}

function _ame_add_sources_if_not_empty(path)
    local source_files = file_utils:path_from_root(path .. "/**.cpp")
    if table.empty(source_files) ~= true then
        add_files(source_files)
    end
end

function _ame_add_headers_if_not_empty(path)
    local header_files = file_utils:path_from_root(path .. "/**.hpp")
    if table.empty(header_files) ~= true then
        add_headerfiles(header_files)
    end
end

function ame_utils:add_library(group, kind, path)
    set_kind(kind)
    set_group(group)

    _ame_add_sources_if_not_empty(path .. "/Private")

    _ame_add_headers_if_not_empty(path .. "/Public")
    _ame_add_headers_if_not_empty(path .. "/Private")

    local include_dirs = file_utils:path_from_root(path .. "/Public")
    local folder_exists = os.isdir(include_dirs)
    if folder_exists == true then
        add_includedirs(include_dirs, {public = true})
    end

    include_dirs = file_utils:path_from_root(path .. "/Private")
    folder_exists = os.isdir(include_dirs)
    if folder_exists == true then
        add_includedirs(include_dirs, {public = false})
    end

    add_filegroups("", {rootdir = "../" .. path .. "/"})
end

function ame_utils:add_binary(group, path)
    ame_utils:add_library(group, "binary", path)
    add_deps("Ame.Engine", {public = true})
end

function ame_utils:add_tests(group, path, target_name)
    local test_files = os.files(file_utils:path_from_root(path .. "/Tests/*/*.cpp"))
    if table.empty(test_files) ~= true then
        for _, file in ipairs(test_files) do
            local file_name = file_utils:get_file_name_without_extension(file)
            target(group .. "_" .. file_name)
                set_group("Tests")
                set_kind("binary")
                add_filegroups("", {rootdir = "../" .. path .. "/Tests/"})

                -- add_defines("BOOST_AUTO_TEST_MAIN")
                add_packages("boost_ut", {public = false})
                add_files(file)
                add_deps(target_name, {public = false})
            target_end()
        end
    end
end

function ame_utils:install_assets()
    after_build(function(target)
        local targetdir = target:targetdir() .. "/Shared/Assets"
        print ("Copying assets to " .. targetdir)
        if not os.isdir(targetdir) then
            print("Creating asset directory")
            os.mkdir(targetdir)
        end
        -- copy folder /Assets to targetdir
        os.cp("Assets/*", targetdir)
    end)
    after_install(function(target)
        local targetdir = target:installdir() .. "/bin/Shared/Assets"
        print ("Copying assets to " .. targetdir)
        if not os.isdir(targetdir) then
            print("Creating asset directory")
            os.mkdir(targetdir)
        end
        -- copy folder /Assets to targetdir
        os.cp("Assets/*", targetdir)
    end)
end
