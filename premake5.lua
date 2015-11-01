workspace "traffic3"
	configurations { "Debug", "Release" }

project "common"
	kind "StaticLib"
	warnings "Extra"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
        buildoptions "-std=c++11 -stdlib=libc++"
	linkoptions "-g"
        files { "src/main/lib/**.h", "src/main/lib/**.cpp" }
	includedirs { "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
        filter "configurations:Debug"
                defines { "DEBUG" }
                flags { "Symbols" }

        filter "configurations:Release"
                defines { "NDEBUG" }
                optimize "On"

project "traffic3"
	kind "ConsoleApp"
        warnings "Extra"
	links { "common" }
	language "C++"
        includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
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
        warnings "Extra"
        links { "common" }
	language "C++"
	targetdir "test/%{cfg.buildcfg}"
        includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/Catch/single_include", "%{cfg.basedir}/3rdparty/hippomocks/HippoMocks", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
        buildoptions "-std=c++11 -stdlib=libc++"
	files { "src/test/**.h", "src/test/**.cpp" }

        filter "configurations:Debug"
                defines { "DEBUG" }
                flags { "Symbols" }

        filter "configurations:Release"
                defines { "NDEBUG" }
                optimize "On"


