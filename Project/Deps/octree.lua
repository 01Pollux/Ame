package("ame.octree")

    set_kind("library", {headeronly = true})
    set_homepage("https://github.com/attcs/Octree")
    set_description("Octree/Quadtree/N-dimensional linear tree.")
    set_license("MIT")

    add_urls("https://github.com/attcs/Octree.git")
    add_versions("v22.04.28", "48dc0708e12a843d912d18a170604d28d7ef4678")

    add_includedirs("include")

    on_install(function (package)
        os.cp("octree.h", package:installdir("include"))
        os.cp("octree_container.h", package:installdir("include"))
        print("Installed to " .. package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <array>
            #include <octree.h>
            #include <octree_container.h>
                        
            using std::array;
            using std::vector;
            using namespace OrthoTree;
            
            void test() {
                auto constexpr points = array{ Point3D{0,0,0}, Point3D{1,1,1}, Point3D{2,2,2} };
                auto const octree = OctreePointC(points, 3 /*max depth*/);

                auto const searchBox = BoundingBox3D{ {0.5, 0.5, 0.5}, {2.5, 2.5, 2.5} };
                auto const pointIDs = octree.RangeSearch(searchBox); //: { 1, 2 }

                auto neighborNo = 2;
                auto pointIDsByKNN = octree.GetNearestNeighbors(Point3D{ 1.1, 1.1, 1.1 }
                , neighborNo
                ); //: { 1, 2 }
            }
        ]]}, {configs = {languages = "c++20"}}))
    end)
package_end()