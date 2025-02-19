workspace "ProyectoLSystems"
    configurations { "Debug", "Release" }
    location "build"

    -- Configuración para Debug y Release
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"

project "LSystemApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    -- Archivos de código fuente
    files {
        "src/**.hpp",
        "src/**.cpp",
        "include/**.hpp",

        "src/**.h",
        "src/**.cc",
        "include/**.h"
    }

    -- Directorios de inclusión
    includedirs {
        "include",
        "C:/SFML/include"
    }

    -- Bibliotecas de SFML (estáticas)
    filter "configurations:Release"
        links {
            "sfml-graphics-s",
            "sfml-window-s",
            "sfml-system-s"
        }
        defines { "SFML_STATIC" }  -- Define este símbolo para compilación estática

    filter "configurations:Debug"
        links {
            "sfml-graphics-s-d",
            "sfml-window-s-d",
            "sfml-system-s-d"
        }
        defines { "SFML_STATIC" }  -- Define este símbolo para compilación estática

    -- Directorios de bibliotecas
    libdirs {
        "C:/SFML/lib"
    }

    -- Configuración específica para Windows
    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        defines { "PLATFORM_WINDOWS" }