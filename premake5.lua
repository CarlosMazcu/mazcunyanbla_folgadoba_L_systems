workspace "ProyectoLSystems"
    platforms {"x64"}
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
        "include/SFML"
    }

    -- Bibliotecas de SFML (estáticas)
    filter "configurations:Release"
        links {
            "sfml-graphics-s",
            "sfml-window-s",
            "sfml-system-s",
            "opengl32",  -- Biblioteca de OpenGL
            "winmm",     -- Biblioteca multimedia de Windows
            "gdi32"      -- Biblioteca GDI de Windows
        }
        defines { "SFML_STATIC" }  -- Define este símbolo para compilación estática

    filter "configurations:Debug"
        links {
            "sfml-graphics-s-d",
            "sfml-window-s-d",
            "sfml-system-s-d",
            "opengl32",  -- Biblioteca de OpenGL
            "winmm",     -- Biblioteca multimedia de Windows
            "gdi32"      -- Biblioteca GDI de Windows
        }
        defines { "SFML_STATIC" }  -- Define este símbolo para compilación estática

    -- Directorios de bibliotecas
    libdirs {
        "lib/SFML"
    }

    -- Configuración específica para Windows
    filter "system:windows"
        systemversion "latest"
        staticruntime "Off"
        defines { "PLATFORM_WINDOWS" }