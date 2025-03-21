workspace "ProyectoLSystems"
    platforms {"x64"}
    configurations { "Debug", "Release" }
    location "build"

    -- Configuración común para Debug y Release
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
        "backends/*.hpp",  -- Los archivos de cabecera en "backends"
        "src/**.h",
        "src/**.cc",
        "include/**.h",
        "backends/*.cpp",  -- Los archivos .cpp en "backends"
        "backends/*.h",  -- Los archivos .cpp en "backends"

        -- Archivos de ImGui
        "backends/imgui/*.cpp",
        "backends/imgui/*.h",
        "backends/ImGuiFileDialog-0.5.2/*.cpp",
        "backends/ImGuiFileDialog-0.5.2/*.h",
        "backends/imgui-sfml/*.cpp",
        "backends/imgui-sfml/*.h"

        
--[[         "backends/imgui/imgui_demo.cpp",  -- Opcional: Demo de ImGui
        "backends/imgui/imgui_draw.cpp",
        "backends/imgui/imgui_widgets.cpp",
        "backends/imgui/imgui_tables.cpp", ]]
        -- Archivos de ImGui-SFML
--[[         "backends/imgui-sfml/imgui-SFML.cpp",
        "backends/imgui-sfml/imgui-SFML.h" ]]
    }

    -- Directorios de inclusión
    includedirs {
        "include",            -- Directorios de inclusión generales
        "include/SFML",       -- Directorio de SFML
        "backends/imgui",     -- Directorio de ImGui en "backends"
        "backends/imgui-sfml", -- Directorio de ImGui-SFML en "backends"
        "backends/ImGuiFileDialog-0.5.2"

    }
    
    defines {
      "IMGUIFILEDIALOG_IMPLEMENTATION"
  }
    -- Configuración de bibliotecas y vinculación
    filter "configurations:Release"
        links {
            "sfml-graphics-s",
            "sfml-window-s",
            "sfml-system-s",
            "opengl32",  -- OpenGL
            "winmm",     -- Windows multimedia
            "gdi32",     -- Windows GDI
--[[             "imgui",     -- Vincular ImGui
            "imgui-sfml" -- Vincular ImGui-SFML ]]
        }
        defines { "SFML_STATIC" }

    filter "configurations:Debug"
        links {
            "sfml-graphics-s-d",
            "sfml-window-s-d",
            "sfml-system-s-d",
            "opengl32",  -- OpenGL
            "winmm",     -- Windows multimedia
            "gdi32",     -- Windows GDI
--[[             "imgui",     -- Vincular ImGui
            "imgui-sfml" -- Vincular ImGui-SFML ]]
        }
        defines { "SFML_STATIC" }

    -- Directorios de bibliotecas
    libdirs {
        "lib/SFML",
        "lib/imgui"
    }

    -- Configuración específica para Windows
    filter "system:windows"
        systemversion "latest"
        staticruntime "Off"
        defines { "PLATFORM_WINDOWS" }
