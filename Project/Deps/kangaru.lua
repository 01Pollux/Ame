package("ame.kangaru")
    set_kind("library", {headeronly = true})
    set_homepage("https://github.com/gracicot/kangaru")
    set_description("🦘 A dependency injection container for C++11, C++14 and later")
    set_license("MIT")
    
    add_urls("https://github.com/gracicot/kangaru.git")

    add_deps("cmake")

    on_install(function (package)
        local configs = {
            "-DKANGARU_REVERSE_DESTRUCTION=ON"
        };
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <kangaru/kangaru.hpp>
            struct Camera {};
            struct Scene {
                Camera& camera;
            };
            struct CameraService : kgr::single_service<Camera> {};
            struct SceneService : kgr::service<Scene, kgr::dependency<CameraService>> {};
            void test() {
                kgr::container container;
                Scene scene = container.service<SceneService>();
                Camera& camera = container.service<CameraService>();
            }
        ]]}, {configs = {languages = "c++11"}}))
    end)
package_end()
