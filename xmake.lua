add_rules("mode.debug", "mode.release")

add_defines("GLM_ENABLE_EXPERIMENTAL")

if is_plat("windows") then
    add_defines("AME_PLATFORM_WINDOWS")
elseif is_plat("linux") then
    add_defines("AME_PLATFORM_LINUX")
end

set_languages("c++latest")

package("glfw")
    set_homepage("https://www.glfw.org/")
    set_description("GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan application development.")
    set_license("zlib")
    set_sourcedir(path.join(os.scriptdir(), "Deps/Externals/glfw"))

    add_deps("cmake")
    if is_plat("macosx") then
        add_frameworks("Cocoa", "IOKit")
    elseif is_plat("windows") then
        add_syslinks("user32", "shell32", "gdi32")
    elseif is_plat("mingw") then
        add_syslinks("gdi32")
    elseif is_plat("linux") then
        add_syslinks("dl", "pthread")
        add_extsources("apt::libglfw3-dev", "pacman::glfw-x11")
    end
    
    add_configs("x11", {description = "Build support for X11", default = is_plat("linux"), type = "boolean"})
    add_configs("wayland", {description = "Build support for Wayland", default = false, type = "boolean"})

    on_load(function (package)
        if package:config("x11") then
            package:add("deps", "libx11", "libxrandr", "libxrender", "libxinerama", "libxfixes", "libxcursor", "libxi", "libxext")
        end
        if package:config("wayland") then
            package:add("deps", "wayland")
        end
    end)

    on_install(function (package)
        local configs = {"-DGLFW_BUILD_DOCS=OFF", "-DGLFW_BUILD_TESTS=OFF", "-DGLFW_BUILD_EXAMPLES=OFF"}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        if package:is_plat("windows") then
            table.insert(configs, "-DUSE_MSVC_RUNTIME_LIBRARY_DLL=" .. (package:config("vs_runtime"):startswith("MT") and "OFF" or "ON"))
        end
        table.insert(configs, "-DGLFW_BUILD_X11=" .. (package:config("x11") and "ON" or "OFF"))
        table.insert(configs, "-DGLFW_BUILD_WAYLAND=" .. (package:config("wayland") and "ON" or "OFF"))
        if package:is_plat("linux") then
            import("package.tools.cmake").install(package, configs, {packagedeps = {"libxrender", "libxfixes", "libxext", "libx11", "wayland"}})
        else
            import("package.tools.cmake").install(package, configs)
        end
    end)

    on_test(function (package)
        assert(package:check_csnippets({test = [[
            void test() {
                glfwInit();
                glfwTerminate();
            }
        ]]}, {configs = {languages = "c11"}, includes = "GLFW/glfw3.h"}))
    end)
package_end()

package("nri")
    set_homepage("https://github.com/NVIDIAGameWorks/NRI")
    set_description("NRI is a low-level abstract render interface which currently supports three backends: D3D11, D3D12 and Vulkan (VK).")
    set_license("MIT")
    set_sourcedir(path.join(os.scriptdir(), "Deps/Externals/nri"))
      
    add_deps("cmake")
    if is_plat("macosx") then
        add_frameworks("Cocoa", "IOKit")
    elseif is_plat("windows") then
        add_syslinks("user32", "gdi32", "dxgi", "dxguid", "d3d12")
    elseif is_plat("mingw") then
        add_syslinks("gdi32")
    elseif is_plat("linux") then
        add_syslinks("dl", "pthread")
    end

    on_load(function (package)
        if package:config("x11") then
            package:add("deps", "libx11", "libxrandr", "libxrender", "libxinerama", "libxfixes", "libxcursor", "libxi", "libxext")
        end
        if package:config("wayland") then
            package:add("deps", "wayland")
        end
        package:add("includedirs", "Include")
    end)
    
    add_configs("x11", {description = "Build support for X11", default = is_plat("linux"), type = "boolean"})
    add_configs("wayland", {description = "Build support for Wayland", default = false, type = "boolean"})
    add_configs("vk", {description = "Build support for Vulkan", default = true, type = "boolean"})
    add_configs("d3d12", {description = "Build support for D3D12", default = true, type = "boolean"})
    add_configs("agility", {description = "Path to Agility SDK", default = "", type = "string"})
    add_configs("shared", {description = "Build shared library", default = true, type = "boolean"})

    on_load(function (package)
        if package:config("x11") then
            package:add("deps", "libx11", "libxrandr", "libxrender", "libxinerama", "libxfixes", "libxcursor", "libxi", "libxext")
        end
        if package:config("wayland") then
            package:add("deps", "wayland")
        end
    end)

    on_install(function (package)
        local configs = {
            "-DNRI_ENABLE_D3D11_SUPPORT=OFF"
        };
        table.insert(configs, "-DDCMAKE_CONFIGURATION_TYPES=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DNRI_ENABLE_VK_SUPPORT=" .. (package:config("vk") and "ON" or "OFF"))
        table.insert(configs, "-DNRI_ENABLE_D3D12_SUPPORT=" .. (package:config("d3d12") and "ON" or "ON"))
        table.insert(configs, "-DNRI_ENABLE_AGILITY_SDK_SUPPORT=" .. (package:config("agility")))
        table.insert(configs, "-DNRI_STATIC_LIBRARY=" .. (package:config("shared") and "OFF" or "ON"))
        table.insert(configs, "-DNRI_ENABLE_XLIB_SUPPORT=" .. (package:config("x11") and "ON" or "OFF"))
        table.insert(configs, "-DNRI_ENABLE_WAYLAND_SUPPORT=" .. (package:config("wayland") and "ON" or "OFF"))
        if package:is_plat("linux") then
            import("package.tools.cmake").install(package, configs, {packagedeps = {"libxrender", "libxfixes", "libxext", "libx11", "wayland"}})
        else
            import("package.tools.cmake").install(package, configs)
        end
    end)
package_end()

add_requires("boost")
add_requires("spdlog")
add_requires("fmt")
add_requires("flecs")
add_requires("cryptopp")
add_requires("magic_enum")
add_requires("glm")
add_requires("concurrencpp")
add_requires("glfw")
add_requires("nri")

target("Core")
    set_group("Ame")
    set_kind("static")
    add_files("Ame/Core/**.cpp")
    add_headerfiles("Ame/Core/**.hpp")

    add_includedirs("Ame/Core/Public", {public = true})
    add_packages(
        "boost",
        "spdlog",
        "fmt",
        "flecs",
        "cryptopp",
        "magic_enum",
        "glm",
        "concurrencpp",
        "glfw",
        {public = true})
target_end()

target("Windowing")
    set_group("Ame")
    set_kind("static")
    add_files("Ame/Windowing/**.cpp")
    add_headerfiles("Ame/Windowing/**.hpp")

    add_includedirs("Ame/Windowing/Public", {public = true})
    add_deps("Core", {public = true})
target_end()

target("Rhi")
    set_group("Ame")
    set_kind("static")
    add_files("Ame/Rhi/**.cpp")
    add_headerfiles("Ame/Rhi/**.hpp")
    add_packages("nri", {public = true})

    add_includedirs("Ame/Rhi/Public", {public = true})
    add_deps("Windowing", {public = true})
target_end()


target("Engine")
    set_group("Ame")
    set_kind("static")
    add_files("Ame/Engine/**.cpp")
    add_headerfiles("Ame/Engine/**.hpp")

    add_includedirs("Ame/Engine/Public", {public = true})
    add_deps("Rhi", {public = true})
target_end()

target("Samples.MultiEngine")
    set_group("Ame")
    set_kind("binary")
    add_files("Samples/MultiEngine/**.cpp")
    add_headerfiles("Samples/MultiEngine/**.hpp")

    add_deps("Engine", {public = true})
target_end()