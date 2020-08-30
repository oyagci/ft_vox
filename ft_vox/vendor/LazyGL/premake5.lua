project "LazyGL"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "sources/graphics/Camera.cpp",
        "sources/graphics/Camera.hpp",
        "sources/graphics/Display.cpp",
        "sources/graphics/Display.hpp",
        "sources/graphics/Mesh.cpp",
        "sources/graphics/Mesh.hpp",
        "sources/graphics/Shader.cpp",
        "sources/graphics/Shader.hpp",
        "sources/graphics/textures/Framebuffer.cpp",
        "sources/graphics/textures/Framebuffer.hpp",
        "sources/inputs/Input.cpp",
        "sources/inputs/Input.hpp",
        "sources/inputs/Keyboard.cpp",
        "sources/inputs/Keyboard.hpp",
        "sources/inputs/Mouse.cpp",
        "sources/inputs/Mouse.hpp",
        "sources/inputs/MouseObserver.hpp",
        "sources/maths/maths.cpp",
        "sources/maths/maths.hpp",
        "sources/maths/transform.hpp",
        "sources/utils/fileutils.cpp",
        "sources/utils/fileutils.hpp",
    }

    includedirs
    {
        "includes/",
        "sources/",
        "sources/graphics",
        "sources/inputs",
        "sources/maths",
        "sources/utils",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
    }

    links
    {
        "GLFW",
        "GLAD",
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Debug"
        symbols "On"