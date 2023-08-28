#pragma once
namespace Plaza::Editor {
    class ProjectGenerator {
    public:
        static void GenerateSolution(const std::string& solutionName, const std::string& projectName, const std::string& outputDirectory);

        static void GenerateProject(const std::string& projectName, const std::string& outputDirectory);
    };
}