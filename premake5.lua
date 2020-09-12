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

include "ft_vox/vendor/glad"
include "ft_vox/vendor/glfw"
include "ft_vox/vendor/LazyGL"
include "ft_vox/vendor/freetype"

project "ft_vox"
    location "ft_vox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/Anchor.cpp",
        "%{prj.name}/src/Anchor.hpp",
        "%{prj.name}/src/Chunk.cpp",
        "%{prj.name}/src/Chunk.hpp",
        "%{prj.name}/src/ChunkFactory.cpp",
        "%{prj.name}/src/ChunkFactory.hpp",
        "%{prj.name}/src/ChunkRenderer.cpp",
        "%{prj.name}/src/ChunkRenderer.hpp",
        "%{prj.name}/src/Cubemap.cpp",
        "%{prj.name}/src/Cubemap.hpp",
        "%{prj.name}/src/FPSCounter.cpp",
        "%{prj.name}/src/FPSCounter.hpp",
        "%{prj.name}/src/Game.cpp",
        "%{prj.name}/src/Game.hpp",
        "%{prj.name}/src/IRenderer.hpp",
        "%{prj.name}/src/main.cpp",
        "%{prj.name}/src/PlayerHUD.cpp",
        "%{prj.name}/src/PlayerHUD.hpp",
        "%{prj.name}/src/Settings.cpp",
        "%{prj.name}/src/Settings.hpp",
        "%{prj.name}/src/SimplexNoise.cpp",
        "%{prj.name}/src/SimplexNoise.hpp",
        "%{prj.name}/src/stb_image.cpp",
        "%{prj.name}/src/stb_image.h",
        "%{prj.name}/src/TextRenderer.cpp",
        "%{prj.name}/src/TextRenderer.hpp",
        "%{prj.name}/src/Texture.cpp",
        "%{prj.name}/src/Texture.hpp",
        "%{prj.name}/src/TextureManager.cpp",
        "%{prj.name}/src/TextureManager.hpp",
        "%{prj.name}/src/Time.cpp",
        "%{prj.name}/src/Time.hpp",
        "%{prj.name}/src/World.cpp",
        "%{prj.name}/src/World.hpp",
        "%{prj.name}/src/WorldGenerator.cpp",
        "%{prj.name}/src/WorldGenerator.hpp",
        "%{prj.name}/src/ui/Button.cpp",
        "%{prj.name}/src/ui/Button.hpp",
        "%{prj.name}/src/ui/Image.hpp",
        "%{prj.name}/src/ui/Label.hpp",
        "%{prj.name}/src/ui/MainMenuBackground.cpp",
        "%{prj.name}/src/ui/MainMenuBackground.hpp",
        "%{prj.name}/src/ui/MainMenuScene.hpp",
        "%{prj.name}/src/ui/PlayerHUDScene.hpp",
        "%{prj.name}/src/ui/SceneComponent.cpp",
        "%{prj.name}/src/ui/SceneComponent.hpp",
        "%{prj.name}/src/ui/TextComponent.hpp",
        "%{prj.name}/src/ui/UI.cpp",
        "%{prj.name}/src/ui/UI.hpp",
        "%{prj.name}/src/ui/UIScene.cpp",
        "%{prj.name}/src/ui/UIScene.hpp",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.LazyGL}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}",
		"%{IncludeDir.freetype}",
    }

    links
    {
        "LazyGL",
		"freetype",
		"GLAD",
		"GLFW",
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