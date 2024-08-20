package("ame.octree")
    set_homepage("https://github.com/attcs/Octree")
    set_description("Octree/Quadtree/N-dimensional linear tree")
    set_license("MIT")
    
    add_urls("https://github.com/attcs/Octree.git", {alias = "git"})

    on_install(function (package)
        io.writefile("xmake.lua", [[
            add_rules("mode.debug", "mode.release")
            target("octree")
                set_kind("headeronly")
                add_headerfiles("octree.h")
                add_headerfiles("octree_container.h")
            target_end()
        ]])
        import("package.tools.xmake").install(package)
    end)
package_end()