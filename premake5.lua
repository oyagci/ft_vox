workspace "ft_vox"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["LazyGL"] = "%{wks.location}/ft_vox/vendor/LazyGL/includes"
IncludeDir["GLFW"]   = "%{wks.location}/ft_vox/vendor/glfw/include"
IncludeDir["glm"]    = "%{wks.location}/ft_vox/vendor/glm"
IncludeDir["GLAD"]   = "%{wks.location}/ft_vox/vendor/glad/include"
IncludeDir["freetype"] = "%{wks.location}/ft_vox/vendor/freetype/include"
IncludeDir["FastNoise"] = "%{wks.location}/ft_vox/vendor/FastNoise"

include "ft_vox/vendor/glad"
include "ft_vox/vendor/glfw"
include "ft_vox/vendor/LazyGL"
include "ft_vox/vendor/freetype"
include "ft_vox/vendor/FastNoise"

project "ft_vox"
    location "ft_vox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.LazyGL}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}",
		"%{IncludeDir.freetype}",
		"%{IncludeDir.FastNoise}",
		"%{IncludeDir.tracy}",
    }

    links
    {
        "LazyGL",
		"freetype",
		"GLAD",
		"GLFW",
		"FastNoise",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

	filter "system:linux"
		cppdialect "C++17"
		staticruntime "On"

	links
	{
		"dl",
		"pthread",
		"X11",
		"zlib",
	}

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Debug"
        symbols "On"