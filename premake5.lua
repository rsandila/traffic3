workspace "traffic3"
  platforms { "32bit", "64bit" }
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
  includedirs { "%{cfg.basedir}/src/main", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
	filter "configurations:Debug"
    defines { "DEBUG" }
    flags { "Symbols" }
    linkoptions "-g"
  filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"
	filter "configurations:32bit"
		architecture ( "x32" )
	filter "configurations:64bit"
		architecture ( "x86_64" )

project "common"
	kind "StaticLib"
	language "C++"
  files { "src/main/lib/**.h", "src/main/lib/**.cpp" }

project "protocol"
  kind "StaticLib"
  language "C++"
  files { "src/main/protocol/**.h", "src/main/protocol/**.cpp" }

project "contentmanager"
  kind "StaticLib"
  language "C++"
  files { "src/main/contentmanager/**.h", "src/main/contentmanager/**.cpp" }

project "rest"
  kind "StaticLib"
  language "C++"
  includedirs { "%{cfg.basedir}/3rdparty/json/src" }
  files { "src/main/rest/**.h", "src/main/rest/**.cpp" }

project "traffic3"
	kind "ConsoleApp"
	links { "common", "rest", "contentmanager", "protocol" }
	language "C++"
  includedirs { "%{cfg.basedir}/src/main", "%{cfg.basedir}/3rdparty/easyloggingpp/src",
    "%{cfg.basedir}/3rdparty/cmdline", "%{cfg.basedir}/src/main/rest" }
	files { "src/main/console/**.h", "src/main/console/**.cpp" }

project "traffic3_test"
	kind "ConsoleApp"
  links { "common", "protocol", "rest", "contentmanager" }
	language "C++"
  includedirs { "%{cfg.basedir}/src/main", "%{cfg.basedir}/3rdparty/Catch/single_include",
    "%{cfg.basedir}/3rdparty/hippomocks/HippoMocks", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
	files { "src/test/lib/**.h", "src/test/lib/**.cpp", "src/test/rest/**.cpp", "src/test/rest/**.h" }
