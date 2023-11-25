#include <limits>
#include <algorithm>

namespace Helpers
{
    namespace Math
    {
        struct Metrics
        {
            int min = std::numeric_limits<int>::max();
            int max = std::numeric_limits<int>::min();
            int sum = 0;
            int total = 0;
            double avg = 0.0;
            Metrics(size_t sz): total(sz) {}

            void ComputeNext(size_t n)
            {
                min = std::min(static_cast<int>(n), min);
                max = std::max(static_cast<int>(n), max);
                sum += n;
            }

            //void Finalize(llvm::StringMap<double>& llvmMap, llvm::StringRef key)
            llvm::StringMap<double> Finalize()
            {
                llvm::StringMap<double> llvmMap;
                double avg = static_cast<double>(sum) / total;
                /*std::string minName =  key.str() + "_min";
                std::string maxName =  key.str() + "_max";
                std::string avgName =  key.str() + "_avg";
                llvmMap[llvm::StringRef(minName)] = min;
                llvmMap[llvm::StringRef(maxName)] = max;
                llvmMap[llvm::StringRef(avgName)] = avg;*/
                llvmMap["minimum"] = min;
                llvmMap["maximum"] = max;
                llvmMap["average"] = avg;

                return llvmMap;
            }

        };
    };
};