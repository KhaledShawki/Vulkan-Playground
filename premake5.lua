workspace  "Vulkan Playground"
    architecture "x64"
    startproject "Vulkan Playground"
    configurations { "Debug", "Release" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	-- Include directories relative to root folder (soulution directory)
    IncludeDir ={}
    IncludeDir["spdlog"] = "Vulkan Playground/lib/spdlog/include"
    IncludeDir["GLFW"] = "Vulkan Playground/lib/GLFW/include"
    IncludeDir["glm"] = "Vulkan Playground/lib/glm"
    IncludeDir["vulkan"] = "Vulkan Playground/lib/Vulkan/Include"

    include "Vulkan Playground/lib/GLFW"
    project "Vulkan Playground"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
        }

        defines
		{
            "PLATFORM_WINDOWS",
            "GLFW_INCLUDE_VULKAN",
            "GLM_FORCE_RADIANS",
            "GLM_FORCE_DEPTH_ZERO_TO_ONE"
        }
        
        includedirs
		{
			"%{prj.name}/src",
			"%{IncludeDir.spdlog}",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.glm}",
			"%{IncludeDir.vulkan}"
        }

        links{
			"GLFW",
			"Vulkan Playground/lib/Vulkan/Lib/vulkan-1.lib"
        }

        filter "system:windows"
			systemversion "latest"

			defines
			{
                "PLATFORM_WINDOWS"
			}

		filter "configurations:Debug"
			defines "DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "RELEASE"
			runtime "Release"
			optimize "on"


