package("ame.freeimage")

    set_homepage("https://sourceforge.net/projects/freeimage/")
    set_description("FreeImage is a library project for developers who would like to support popular graphics image formats (PNG, JPEG, TIFF, BMP and others).")

    set_sourcedir("ThirdParty/FreeImage")

    add_configs("shared", {description = "Build shared library.", default = true})
    add_configs("rgb", {description = "Use RGB instead of BGR.", default = false})

    on_load("windows", function (package)
        if not package:config("shared") then
            package:add("defines", "FREEIMAGE_LIB")
        end
    end)

    on_install("windows|x64", "windows|x86", "macosx", "linux", function (package)
        if package:is_plat("windows") and package:is_arch("x86") then
            local vs = import("core.tool.toolchain").load("msvc"):config("vs")
            if tonumber(vs) < 2019 then
                raise("Your compiler is too old to use this library.")
            end
        end
        local sources, includes
        local content = io.readfile("Makefile.srcs")
        sources = content:match("SRCS = (.-)\n"):split(" ")
        includes = content:match("INCLUDE = (.-)\n"):gsub("%-I", ""):split(" ")
        defines = package:config("rgb") and '"FREEIMAGE_COLORORDER=1"' or '"FREEIMAGE_COLORORDER=0"'
        io.writefile("xmake.lua", format([[
            add_rules("mode.debug", "mode.release")
            includes("check_cincludes.lua")
            target("freeimage")
                set_kind("$(kind)")
                set_languages("c++11")
                add_files({"%s"})
                add_headerfiles("Source/FreeImage.h", "Source/FreeImageIO.h")
                set_symbols("hidden")
                add_includedirs({"%s"})
                check_cincludes("Z_HAVE_UNISTD_H", "unistd.h")
                add_defines("OPJ_STATIC", "NO_LCMS", "LIBRAW_NODLL", "DISABLE_PERF_MEASUREMENT", ]] .. defines .. [[)
                if is_plat("windows") then
                    add_files("FreeImage.rc")
                    add_defines("WIN32", "_CRT_SECURE_NO_DEPRECATE")
                    add_defines(is_kind("static") and "FREEIMAGE_LIB" or "FREEIMAGE_EXPORTS")
                else
                    add_defines("__ANSI__")
                end
        ]], table.concat(sources, "\",\""), table.concat(includes, "\", \"")))
        import("package.tools.xmake").install(package)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("FreeImage_Initialise", {includes = "FreeImage.h"}))
    end)