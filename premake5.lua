workspace "traffic3"
	architecture ( "x86_64" )
	configurations { "Debug", "Release" }
	warnings "Extra"
	if os.is("macosx") then
		buildoptions "-std=c++11 -stdlib=libc++"
	else
		if not os.is("windows") then
		   buildoptions "-std=c++0x"
		   links { "pthread" }
		else
			links { "Ws2_32" }
		end
	end
	flags { "StaticRuntime", "MultiProcessorCompile" }
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
  includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/easyloggingpp/src", "%{cfg.basedir}/3rdparty/cmdline" }
  targetdir "bin/%{cfg.buildcfg}"
	files { "src/main/console/**.h", "src/main/console/**.cpp" }

project "traffic3_test"
	kind "ConsoleApp"
  links { "common" }
	language "C++"
	targetdir "test/%{cfg.buildcfg}"
  includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/Catch/single_include", "%{cfg.basedir}/3rdparty/hippomocks/HippoMocks", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
	files { "src/test/**.h", "src/test/**.cpp" }
