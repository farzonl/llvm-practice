#ifndef LLVM_PRACTICE_JSON_LLVM_HELPER_H
#define LLVM_PRACTICE_JSON_LLVM_HELPER_H

#include "JsonHelpers.h"

using json = nlohmann::json;

namespace Helpers
{
    class JsonLLVMHelpers : public JsonHelpers
    {
    public:
        JsonLLVMHelpers() = delete;

        template< typename T >
        static json extractAsJson(const llvm::StringMap<T>& llvmMap)
        {
            json j;
            for (auto &item : llvmMap) {
                j[item.first()] = item.second;
            }
            return j;
        }

        template< typename T >
        static void writeMapToJson(const llvm::StringMap<T>& llvmMap, 
                                   const std::string& filename)
        {
            json j = extractAsJson(llvmMap);
            writeJsonToFile(j, filename);
        }
    };
};

#endif // LLVM_PRACTICE_JSON_LLVM_HELPER_H
