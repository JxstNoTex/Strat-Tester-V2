

dependencies = {
	basePath = "./deps"
}

function dependencies.load()
	dir = path.join(dependencies.basePath, "premake/*.lua")
	deps = os.matchfiles(dir)

	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function dependencies.imports()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function dependencies.projects()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end



dependencies.load()

workspace "Strat Tester 2.0"
	startproject "Strat-Tester 2.0"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"

	configurations {"Debug", "Release", "Dist"}

	language "C++"
	cppdialect "C++20"

	architecture "x86_64"
	platforms "x64"

	systemversion "latest"
	symbols "On"
	staticruntime "On"
	editandcontinue "Off"
	warnings "Extra"
	characterset "ASCII"

	

	flags {"NoIncrementalLink", "NoMinimalRebuild", "MultiProcessorCompile", "No64BitChecks"}

	filter "platforms:x64"
		defines {"_WINDOWS", "WIN32"}
	filter {}

	filter "configurations:Release"
		optimize "Size"
		buildoptions {"/GL"}
		linkoptions {"/IGNORE:4702", "/LTCG"}
		defines {"NDEBUG"}
		flags {"FatalCompileWarnings"}
	filter {}

	filter "configurations:Debug"
		optimize "Debug"
		defines {"DEBUG", "_DEBUG"}
	filter {}

project "common"
	kind "StaticLib"
	language "C++"

	files {"./src/common/**.hpp", "./src/common/**.cpp"}

	includedirs {"./src/common", "%{prj.location}/src"}

	resincludedirs {"$(ProjectDir)src"}

	dependencies.imports()

project "Strat Tester 2.0"
	kind "SharedLib"
	language "C++"

	targetname "strat-tester-2.0"

	pchheader "std_include.hpp"


	files {"./src/strat-tester-dll/**.rc", "./src/strat-tester-dll/**.hpp", "./src/strat-tester-dll/**.cpp", "./src/strat-tester-dll/resources/**.*" }

	includedirs {"./src/strat-tester-dll", "./src/common","%{prj.location}/src"}

	resincludedirs {"$(ProjectDir)src"}

	links {"common"}

	dependencies.imports()


group "Dependencies"
	dependencies.projects()
