#include "Engine/Core/PreCompiledHeaders.h"
#include "ProjectGenerator.h"

namespace Plaza::Editor {
	void ProjectGenerator::GenerateSolution(const std::string& solutionName, const std::string& projectName, const std::string& outputDirectory) {
        std::filesystem::create_directories(outputDirectory);
        std::ofstream solutionFile(outputDirectory + "/" + solutionName + ".sln");
        if (!solutionFile.is_open()) {
            std::cerr << "Error opening solution file." << std::endl;
            return;
        }

        solutionFile << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        solutionFile << "# Visual Studio 2023\n";
        solutionFile << "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" << projectName << "\", \"" << projectName << ".vcxproj\", \"{SOME_GUID}\"\n";
        solutionFile << "EndProject\n";
        solutionFile << "Global\n";
        solutionFile << "    GlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
        solutionFile << "        Debug|Win32 = Debug|Win32\n";
        solutionFile << "        Release|Win32 = Release|Win32\n";
        solutionFile << "    EndGlobalSection\n";
        solutionFile << "    GlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
        solutionFile << "        {SOME_GUID}.Debug|Win32.ActiveCfg = Debug|Win32\n";
        solutionFile << "        {SOME_GUID}.Debug|Win32.Build.0 = Debug|Win32\n";
        solutionFile << "        {SOME_GUID}.Release|Win32.ActiveCfg = Release|Win32\n";
        solutionFile << "        {SOME_GUID}.Release|Win32.Build.0 = Release|Win32\n";
        solutionFile << "    EndGlobalSection\n";
        solutionFile << "EndGlobal\n";
    }

    void ProjectGenerator::GenerateProject(const std::string& projectName, const std::string& outputDirectory) {
        std::ofstream projectFile(outputDirectory + "/" + projectName + ".vcxproj");
        if (!projectFile.is_open()) {
            std::cerr << "Error opening project file." << std::endl;
            return;
        }

        projectFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        projectFile << "<Project DefaultTargets=\"Build\" ToolsVersion=\"16.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
        projectFile << "    <ItemGroup Label=\"ProjectConfigurations\">\n";
        projectFile << "        <ProjectConfiguration Include=\"Debug|Win32\">\n";
        projectFile << "            <Configuration>Debug</Configuration>\n";
        projectFile << "            <Platform>Win32</Platform>\n";
        projectFile << "        </ProjectConfiguration>\n";
        projectFile << "    </ItemGroup>\n";
        projectFile << "    <PropertyGroup Label=\"Globals\">\n";
        projectFile << "        <ProjectGuid>{SOME_GUID}</ProjectGuid>\n";
        projectFile << "        <Keyword>Win32Proj</Keyword>\n";
        projectFile << "    </PropertyGroup>\n";
        projectFile << "    <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n";
        projectFile << "    <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\" Label=\"Configuration\">\n";
        projectFile << "        <ConfigurationType>Application</ConfigurationType>\n";
        projectFile << "        <UseDebugLibraries>true</UseDebugLibraries>\n";
        projectFile << "        <CharacterSet>Unicode</CharacterSet>\n";
        projectFile << "    </PropertyGroup>\n";
        projectFile << "    <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n";
        projectFile << "    <ImportGroup Label=\"ExtensionSettings\">\n";
        projectFile << "    </ImportGroup>\n";
        projectFile << "    <ImportGroup Label=\"Shared\">\n";
        projectFile << "    </ImportGroup>\n";
        projectFile << "    <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\n";
        projectFile << "    </ImportGroup>\n";
        projectFile << "    <PropertyGroup Label=\"UserMacros\" />\n";
        projectFile << "    <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\n";
        projectFile << "    </PropertyGroup>\n";
        projectFile << "    <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\n";
        projectFile << "        <ClCompile>\n";
        projectFile << "            <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
        projectFile << "            <RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>\n";
        projectFile << "        </ClCompile>\n";
        projectFile << "        <Link>\n";
        projectFile << "            <SubSystem>Console</SubSystem>\n";
        projectFile << "        </Link>\n";
        projectFile << "    </ItemDefinitionGroup>\n";
        projectFile << "    <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n";
        projectFile << "    <ImportGroup Label=\"ExtensionTargets\">\n";
        projectFile << "    </ImportGroup>\n";
        projectFile << "</Project>\n";
    }
}