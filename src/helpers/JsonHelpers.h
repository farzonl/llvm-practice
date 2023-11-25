#ifndef LLVM_PRACTICE_JSON_HELPER_H
#define LLVM_PRACTICE_JSON_HELPER_H
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace Helpers
{
    class JsonHelpers
    {
    public:
        JsonHelpers() = delete;
        static void writeJsonToFile(const json& j, const std::string& filename) 
        {
            
            std::ofstream o(filename+".json");
            o << std::setw(4) << j << std::endl;
        }

        static json readJsonFile(const std::string& filename)
        {
            std::ifstream ifs(filename);
            return json::parse(ifs);
        }
    };
};

#endif // LLVM_PRACTICE_JSON_HELPER_H
