workspace "traffic3"
	configurations { "Debug", "Release" }

project "traffic3"
	kind "ConsoleApp"
	language "C++"
        targetdir "bin/%{cfg.buildcfg}"
	buildoptions "-std=c++11 -stdlib=libc++"
	files { "src/main/**.h", "src/main/**.cpp" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"



