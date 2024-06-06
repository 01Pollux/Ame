rule("mode.Debug")
    on_config(function (target)
        if is_mode("Debug") then
            if not target:get("symbols") then
                target:set("symbols", "debug")
            end
            if not target:get("optimize") then
                target:set("optimize", "none")
            end
            target:add("defines", "AME_DEBUG")
            target:add("defines", "AME_ASSET_MGR_DISABLE_HASH_VALIDATION")
        end
    end)
rule_end()

rule("mode.Release")
    on_config(function (target)
        if is_mode("Release") then
            if not target:get("symbols") then
                target:set("symbols", "debug")
            end
            if not target:get("optimize") then
                if target:is_plat("android", "iphoneos") then
                    target:set("optimize", "smallest")
                else
                    target:set("optimize", "fastest")
                end
            end
            if not target:get("strip") then
                target:set("strip", "all")
            end
            target:add("cxflags", "-DNDEBUG")
            target:add("defines", "AME_ASSET_MGR_DISABLE_HASH_VALIDATION")
            target:add("defines", "AME_RELEASE")
        end
    end)
rule_end()

rule("mode.Dist")
    on_config(function (target)
        if is_mode("Dist") then
            if not target:get("symbols") and target:kind() ~= "shared" then
                target:set("symbols", "hidden")
            end
            if not target:get("optimize") then
                if target:is_plat("android", "iphoneos") then
                    target:set("optimize", "smallest")
                else
                    target:set("optimize", "fastest")
                end
            end
            if not target:get("strip") then
                target:set("strip", "all")
            end
            target:add("cxflags", "-DNDEBUG")
            target:add("defines", "AME_ASSET_MGR_DISABLE_HASH_VALIDATION")
            target:add("defines", "AME_DIST")
        end
    end)
rule_end()
