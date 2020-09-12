project "FastNoise"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
		"FastNoise.cpp"
    }

    includedirs
    {
		"."
	}

    links
    {
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Debug"
        symbols "On"