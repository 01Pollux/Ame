ame_csharp_utils = {}

function ame_csharp_utils:add_library(name, group, path, output_path, callback)
    target(name)
        set_kind("phony")
        set_group(group)

        if callback ~= nil then
            callback()
        end

        on_build(function (target)
            local target_dir =  target:targetdir()
            if output_path ~= nil then
                target_dir =  target_dir .. "/" .. output_path
            end

            local mode = "Debug"
            if is_mode("release") then
                mode = "Release"
            end

            os.execv("dotnet", {
                "build", path,
                "--configuration", mode,
                "--nologo",
                "--verbosity", "minimal",
                "--no-restore",
                "--output", target_dir
            })
        end)

        on_install(function (target)
            local target_dir =  target:targetdir()
            if output_path ~= nil then
                target_dir =  target_dir .. "/" .. output_path
            end

            local mode = "Debug"
            if is_mode("release") then
                mode = "Release"
            end

            os.execv("dotnet", {
                "publish", path,
                "--configuration", "Release",
                "--nologo",
                "--verbosity", "minimal",
                "--no-restore",
                "--output", target_dir
            })
        end)
    target_end()
end