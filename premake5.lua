workspace "traffic3"
	configurations { "Debug", "Release" }
	warnings "Extra"
	buildoptions "-std=c++11 -stdlib=libc++"
	filter "configurations:Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
  filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "common"
	kind "StaticLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	linkoptions "-g"
  files { "src/main/lib/**.h", "src/main/lib/**.cpp" }
	includedirs { "%{cfg.basedir}/3rdparty/easyloggingpp/src" }

project "traffic3"
	kind "ConsoleApp"
	links { "common" }
	language "C++"
  includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
  targetdir "bin/%{cfg.buildcfg}"
	files { "src/main/console/**.h", "src/main/console/**.cpp" }

project "traffic3_test"
	kind "ConsoleApp"
  links { "common" }
	language "C++"
	targetdir "test/%{cfg.buildcfg}"
  includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/Catch/single_include", "%{cfg.basedir}/3rdparty/hippomocks/HippoMocks", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
	files { "src/test/**.h", "src/test/**.cpp" }
