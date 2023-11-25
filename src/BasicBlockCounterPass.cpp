#include "BasicBlockCounterPass.h"
#include "helpers/JsonLLVMHelpers.h"
#include "helpers/MathHelpers.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

// Pretty-prints the result of this analysis
static void printBasicBlockCounterPassResult(llvm::raw_ostream &,
                              const ResultBasicBlockPairPass &OC);

llvm::AnalysisKey BasicBlockCounterPass::Key;

BasicBlockCounterPass::Result BasicBlockCounterPass::getBasicBlockInfo(const llvm::Module &M) const {
  BasicBlockFunctionMap bbFunctionMap;
  
  Helpers::Math::Metrics metrics(M.size());
  
  for (auto &Func : M) {
    bbFunctionMap[Func.getName()] = Func.size();
    metrics.ComputeNext(Func.size());
  }

  BasicBlockMetrics bbMetrics = metrics.Finalize();
  return BasicBlockCounterPass::Result(bbFunctionMap, bbMetrics);
}

BasicBlockCounterPass::Result BasicBlockCounterPass::run(llvm::Module &M,
                                         llvm::ModuleAnalysisManager &) {
  return getBasicBlockInfo(M);
}

PreservedAnalyses BasicBlockCounterPassPrinter::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &MAM) {
                                          
  auto &BasicBlockPair = MAM.getResult<BasicBlockCounterPass>(M);
  printBasicBlockCounterPassResult(OS, BasicBlockPair);
  return PreservedAnalyses::all();
}

PreservedAnalyses BasicBlockCounterPassToJson::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &MAM) {
                                          
  auto &BasicBlockPair = MAM.getResult<BasicBlockCounterPass>(M);
  
  nlohmann::json result;
  nlohmann::json modulePair;
  nlohmann::json functions = Helpers::JsonLLVMHelpers::extractAsJson(BasicBlockPair.first);
  nlohmann::json metrics =   Helpers::JsonLLVMHelpers::extractAsJson(BasicBlockPair.second);

  modulePair["functions"] = functions;
  modulePair["metrics"] = metrics;
  result[M.getName()] = modulePair;
  
  Helpers::JsonLLVMHelpers::writeJsonToFile(result, "BasicBlockCount");
  return PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getBasicBlockCounterPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "BasicBlockCounterPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            // #1 REGISTRATION FOR "opt -passes=print<bb-count-pass>"
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print<bb-count-pass>") {
                    MPM.addPass(BasicBlockCounterPassPrinter(llvm::errs()));
                    return true;
                  }
                  return false;
                });
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "json<bb-count-pass>") {
                    MPM.addPass(BasicBlockCounterPassToJson());
                    return true;
                  }
                  return false;
            });
            // #2 REGISTRATION FOR "MAM.getResult<StaticCallCounter>(Module)"
            PB.registerAnalysisRegistrationCallback(
                [](ModuleAnalysisManager &MAM) {
                  MAM.registerPass([&] { return BasicBlockCounterPass(); });
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getBasicBlockCounterPassPluginInfo();
}

//------------------------------------------------------------------------------
// Helper functions - implementation
//------------------------------------------------------------------------------
static void printBasicBlockCounterPassResult(raw_ostream &OutS,
                                     const ResultBasicBlockPairPass &BasicBlockPair) {
  
  BasicBlockFunctionMap bbMap = BasicBlockPair.first;
  OutS << "================================================="
               << "\n";
  OutS << "LLVM-Practice: BasicBlockCounterPass results\n";
  OutS << "=================================================\n";
  const char *str1 = "Function Name";
  const char *str2 = "BB Count";
  OutS << format("%-40s %-10s\n", str1, str2);
  OutS << "-------------------------------------------------"
               << "\n";
  for (auto &bb : bbMap) {
    OutS << format("%-40s %-10lu\n", bb.first().str().c_str(),
                           bb.second);
  }
  OutS << "-------------------------------------------------"
               << "\n\n";
}