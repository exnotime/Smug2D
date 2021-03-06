solution "Smug2D"
    configurations { "Debug", "Release" }
        flags{ "NoPCH" }
        includedirs { "include" }
        libdirs {"libs"}
        platforms{"x64" }

        local location_path = "solution"
        if _ACTION then
	        defines { "_CRT_SECURE_NO_WARNINGS", "NOMINMAX"  }
            location_path = location_path .. "/" .. _ACTION
            location ( location_path )
            location_path = location_path .. "/projects"
        end
    disablewarnings { "4251" }
    systemversion ("10.0") --use latest
    configuration { "Debug" }
        defines { "DEBUG" }
        symbols "On"
        targetdir ( "bin/" .. "/debug" )

    configuration { "Release" }
        defines { "NDEBUG", "RELEASE" }
        optimize "on"
        floatingpoint "fast"
        targetdir ( "bin/" .. "/release" )

	project "Core"
        targetname "Smug2D"
		debugdir ""
        defines { "AS_USE_NAMESPACE"}
		location ( location_path )
		language "C++"
		kind "ConsoleApp"
		files { "src/Core/**"}
        includedirs { "include", "src/Core" }
        staticruntime "On" 
        links { "Box2D" }
        configuration { "Debug" }
                links { "angelscript64d", "sfml-main-d", "sfml-system-d", "sfml-graphics-d", "sfml-window-d","OpenGL32", "Ws2_32" }
        configuration { "Release" }
                links { "angelscript64", "sfml-main", "sfml-system", "sfml-graphics", "sfml-window", "OpenGL32", "Ws2_32" }

    project "Box2D"
        targetname "Box2D"
        debugdir ""
        location ( location_path )
        language "C++"
        kind "StaticLib"
        files { "src/Box2D/**"}
        includedirs { "include", "src/Box2D" }
        staticruntime "On"
