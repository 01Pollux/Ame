ame_utils = {}

function __ame_add_files_if_not_empty(path, opts)
    opts = opts or {}
    add_files(path, opts)
end

function __ame_add_headerfiles_if_not_empty(path, opts)
    opts = opts or {}
    add_headerfiles(path, opts)
end

function _ame_add_sources_if_not_empty(path)
    __ame_add_files_if_not_empty(file_utils:path_from_root(path .. "/**.cpp"))
end

function _ame_add_headers_if_not_empty(path)
    __ame_add_headerfiles_if_not_empty(file_utils:path_from_root(path .. "/**.hpp"))
    __ame_add_headerfiles_if_not_empty(file_utils:path_from_root(path .. "/**.h"))
end

function ame_utils:add_library(name, group, kind, path, callback)
    target(name)
        set_kind(kind)
        set_group(group)

        if kind ~= "headeronly" then
            _ame_add_sources_if_not_empty(path .. "/Private")
            _ame_add_headers_if_not_empty(path .. "/Private")
        end
        _ame_add_headers_if_not_empty(path .. "/Public")

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

        if callback ~= nil then
            callback()
        end
    target_end()
end

function ame_utils:add_binary(name, group, path, callback)
    self:add_library(name, group, "binary", path, function()
        add_deps("AmeEngine", {public = true})
        if callback ~= nil then
            callback()
        end
    end)
end

function ame_utils:add_plugin(name, group, path, callback)
    self:add_library(name, group, "shared", path, function()
        add_deps("AmeEngine", {public = true})
        after_load(function(target)
            target:set("targetdir", target:targetdir() .. "/Plugins")
        end)
        after_install(function(target)
            local plugins_dir = target:installdir() .. "/bin/Plugins"
            local src_plugin = target:installdir() .. target:name()
            if not os.isdir(plugins_dir) then
                os.mkdir(plugins_dir)
            end
            os.mv(target:targetfile(), plugins_dir)
        end)
        if callback ~= nil then
            callback()
        end
    end)
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
        print("Copying assets to " .. targetdir)
        if not os.isdir(targetdir) then
            print("Creating asset directory")
            os.mkdir(targetdir)
        end
        -- copy folder /Assets to targetdir
        os.cp("Assets/*", targetdir)
    end)
    after_install(function(target)
        local targetdir = target:installdir() .. "/bin/Shared/Assets"
        print("Copying assets to " .. targetdir)
        if not os.isdir(targetdir) then
            print("Creating asset directory")
            os.mkdir(targetdir)
        end
        -- copy folder /Assets to targetdir
        os.cp("Assets/*", targetdir)
    end)
end

function ame_utils:copy_to_plugins()
    local callback = function(target, move)
        local dll_path = target:targetfile()
        local plugins_dir = path.join(target:targetdir(), "Plugins")
        if not os.isdir(plugins_dir) then
            os.mkdir(plugins_dir)
        end
        if move then
            os.mv(dll_path, plugins_dir)
            print("DLL moved to " .. plugins_dir)
        else
            os.cp(dll_path, plugins_dir)
            print("DLL copied to " .. plugins_dir)
        end
    end

    after_build(function (target)
        local dll_path = target:targetfile()
        local plugins_dir = path.join(target:targetdir(), "Plugins")
        if not os.isdir(plugins_dir) then
            os.mkdir(plugins_dir)
        end
        os.cp(dll_path, plugins_dir)
        print("DLL copied to " .. plugins_dir)
    end)
    after_install(function (target)
        local dll_path = target:targetfile()
        local plugins_dir = path.join(target:targetdir(), "Plugins")
        if not os.isdir(plugins_dir) then
            os.mkdir(plugins_dir)
        end
        os.mv(dll_path, plugins_dir)
        print("DLL moved to " .. plugins_dir)
    end)
end
