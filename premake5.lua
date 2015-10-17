workspace "traffic3"
	configurations { "Debug", "Release" }

project "common"
	kind "StaticLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
        buildoptions "-std=c++11 -stdlib=libc++"
        files { "src/main/lib/**.h", "src/main/lib/**.cpp" }

        filter "configurations:Debug"
                defines { "DEBUG" }
                flags { "Symbols" }

        filter "configurations:Release"
                defines { "NDEBUG" }
                optimize "On"

project "traffic3"
	kind "ConsoleApp"
	links { "common" }
	language "C++"
        includedirs { "%{cfg.basedir}/src/main/lib" }
        targetdir "bin/%{cfg.buildcfg}"
	buildoptions "-std=c++11 -stdlib=libc++"
	files { "src/main/console/**.h", "src/main/console/**.cpp" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "traffic3_test"
	kind "ConsoleApp"
        links { "common" }
	language "C++"
	targetdir "test/%{cfg.buildcfg}"
        includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/Catch/single_include", "%{cfg.basedir}/3rdparty/FakeIt/single_header/catch" }
        buildoptions "-std=c++11 -stdlib=libc++"
	files { "src/test/**.h", "src/test/**.cpp" }

        filter "configurations:Debug"
                defines { "DEBUG" }
                flags { "Symbols" }

        filter "configurations:Release"
                defines { "NDEBUG" }
                optimize "On"


