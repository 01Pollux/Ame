ame_csharp_utils = {}

function ame_csharp_utils:add_library(name, group, kind, path, output_path, callback)
    target(name)
        set_kind(kind)
        set_group(group)

        add_rules("csharp")

        add_extrafiles(file_utils:path_from_root(path .. "/**.cs"))
        add_extrafiles(file_utils:path_from_root(path .. "/**.csproj"))

        remove_extrafiles(file_utils:path_from_root(path .. "/bin/**"))
        remove_extrafiles(file_utils:path_from_root(path .. "/obj/**"))

        add_filegroups("", {rootdir = "../" .. path .. "/"})

        local mode = "Debug"
        if is_mode("release") then
            mode = "Release"
        end

        if callback ~= nil then
            callback()
        end

        on_build(function (target)
            local target_path =  target:targetdir()
            if output_path ~= nil then
                target_path =  target_path .. "/" .. output_path
            end

            os.execv("dotnet", {
                "build", path,
                "--configuration", mode,
                "--output", target_path,
                "--nologo",
                "--verbosity", "minimal",
            })
        end)
        on_clean(function (target)
            os.execv("dotnet", {
                "clean", path,
                "--nologo",
                "--verbosity", "minimal",
            })
        end)
        on_run(function (target)
            os.execv("dotnet", {
                "run", path,
                "--configuration", mode,
                "--nologo",
                "--verbosity", "minimal",
            })
        end)
        on_install(function (target)
            local install_path =  target:installdir() .. "/bin"
            if output_path ~= nil then
                install_path =  install_path .. "/" .. output_path
            end
            os.execv("dotnet", {
                "publish", path,
                "--configuration", mode,
                "--output", install_path,
                "--nologo",
                "--verbosity", "minimal",
            })
        end)
    target_end()
end

function ame_csharp_utils:add_binary(name, group, path, output_path, callback)
    self:add_library(name, group, "binary", path, output_path, function()
        if callback ~= nil then
            callback()
        end
        add_deps("AmeEngine", {public = true})
    end)
end