target("SimpleLog")
    ame_header_executable("Samples/Engine", "Simple Log", "Samples/Engine/SimpleLog")
target_end()

target("SimpleWindow")
    ame_header_executable("Samples/Engine", "Simple Window", "Samples/Engine/SimpleWindow")
target_end()

target("MultiEngine")
    ame_header_executable("Samples/Engine", "MultiEngine", "Samples/Engine/MultiEngine")
target_end()

target("MultiRhi OneEngine")
    ame_header_executable("Samples/Engine", "MultiRhi OneEngine", "Samples/Engine/MultiRhi OneEngine")
target_end()

target("MultiRhi MultiEngine")
    ame_header_executable("Samples/Engine", "MultiRhi MultiEngine", "Samples/Engine/MultiRhi MultiEngine")
target_end()

--

target("Triangle")
    ame_header_executable("Samples/Rhi", "Triangle", "Samples/Rhi/Triangle")
target_end()

--

target("FlappyRocket")
    ame_header_executable("Samples/Games", "FlappyRocket", "Samples/Games/FlappyRocket")
    after_build(function(target)
        local targetdir = target:targetdir() .. "/Shared/Assets"
        print ("Copying assets to " .. targetdir)
        if not os.isdir(targetdir) then
            print("Creating asset directory")
            os.mkdir(targetdir)
        end
        -- copy folder /Assets to targetdir
        os.cp("Assets/(**)", targetdir)
    end)
    after_install(function(target)
        local targetdir = target:installdir() .. "/bin/Shared/Assets"
        print ("Copying assets to " .. targetdir)
        if not os.isdir(targetdir) then
            print("Creating asset directory")
            os.mkdir(targetdir)
        end
        -- copy folder /Assets to targetdir
        os.cp("Assets/(**)", targetdir)
    end)
target_end()
