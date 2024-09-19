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

        local run_dotnet_cmd = function (os, target, cmd)
            local mode = "Debug"
            if is_mode("release") then
                mode = "Release"
            end
    
            os.execv("dotnet", {
                cmd, file_utils:path_from_root(path),
                "--configuration", mode,
                "--nologo",
                "--verbosity", "minimal",
                "--no-restore",
                "--output", target:targetdir()
            })
        end 
    
        on_build(function (target)
            run_dotnet_cmd(os, target, "build")
        end)
        on_build(function (target)
            run_dotnet_cmd(os, target, "clean")
        end)
        on_build(function (target)
            run_dotnet_cmd(os, target, "run")
        end)
        on_build(function (target)
            run_dotnet_cmd(os, target, "publish")
        end)

        if callback ~= nil then
            callback()
        end
    target_end()
end

function ame_csharp_utils:add_binary(name, group, path, callback)
    self:add_library(name, group, "binary", path, function()
        add_deps("AmeEngine", {public = true})
        if callback ~= nil then
            callback()
        end
    end)
end