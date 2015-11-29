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
	includedirs { "%{cfg.basedir}/3rdparty/easyloggingpp/src" }

project "rest"
  kind "StaticLib"
  language "C++"
  files { "src/main/rest/**.h", "src/main/rest/**.cpp" }
	includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }

project "traffic3"
	kind "ConsoleApp"
	links { "common", "rest" }
	language "C++"
  includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/easyloggingpp/src",
    "%{cfg.basedir}/3rdparty/cmdline", "%{cfg.basedir}/src/main/rest" }
	files { "src/main/console/**.h", "src/main/console/**.cpp" }

project "traffic3_test"
	kind "ConsoleApp"
  links { "common" }
	language "C++"
  includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/3rdparty/Catch/single_include", "%{cfg.basedir}/3rdparty/hippomocks/HippoMocks", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
	files { "src/test/lib/**.h", "src/test/lib/**.cpp" }

project "rest_test"
  kind "ConsoleApp"
  links { "common", "rest" }
  language "C++"
  includedirs { "%{cfg.basedir}/src/main/lib", "%{cfg.basedir}/src/main/rest", "%{cfg.basedir}/3rdparty/Catch/single_include", "%{cfg.basedir}/3rdparty/hippomocks/HippoMocks", "%{cfg.basedir}/3rdparty/easyloggingpp/src" }
  files { "src/test/rest/**.h", "src/test/rest/**.cpp" }
