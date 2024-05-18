package("ame.nri")
    set_kind("library")
    set_homepage("https://github.com/NVIDIAGameWorks/NRI")
    set_description("NRI is a low-level abstract render interface which currently supports three backends: D3D11, D3D12 and Vulkan (VK).")
    set_license("MIT")

    add_urls("https://github.com/NVIDIAGameWorks/NRI.git")

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
        local text = [[

# Export NVAPI
if (INPUT_LIB_NVAPI_FOUND)
    add_library (NVAPI INTERFACE)
    target_link_libraries (NVAPI INTERFACE ${INPUT_LIB_NVAPI})
endif ()

# Install
include(GNUInstallDirs)

install (TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install (DIRECTORY "Include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install (DIRECTORY "Resources/" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME})
        ]]
        
        local file = io.open("CMakeLists.txt", "a")
        file:write(text)
        file:close()

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