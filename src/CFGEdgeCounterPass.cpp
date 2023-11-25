#include "CFGEdgeCounterPass.h"
#include "helpers/JsonLLVMHelpers.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

// Pretty-prints the result of this analysis
static void printCFGEdgeCounterPassResult(llvm::raw_ostream &,
                              const ResultCFGEdgeCounterPass &OC);

llvm::AnalysisKey CFGEdgeCounterPass::Key;

CFGEdgeCounterPass::Result CFGEdgeCounterPass::getCFGEdgeInfo(const llvm::Module &M) const {
  CFGEdgeCounterPass::Result CFGEdgeMap;
  for (auto &Func : M) {
    int numEdges = 0;
    for (auto& block : llvm::make_range(Func.begin(), Func.end())) {
        // Count the successors of the current block
        numEdges += std::distance(succ_begin(&block), succ_end(&block));
    }
    CFGEdgeMap[Func.getName()] = numEdges;
  }

  return CFGEdgeMap;
}

CFGEdgeCounterPass::Result CFGEdgeCounterPass::run(llvm::Module &M,
                                         llvm::ModuleAnalysisManager &) {
  return getCFGEdgeInfo(M);
}

PreservedAnalyses CFGEdgeCounterPassPrinter::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &MAM) {
                                          
  auto &CFGEdgeMap = MAM.getResult<CFGEdgeCounterPass>(M);
  printCFGEdgeCounterPassResult(OS, CFGEdgeMap);
  return PreservedAnalyses::all();
}

PreservedAnalyses CFGEdgeCounterPassToJson::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &MAM) {
                                          
  auto &CFGEdgeMap = MAM.getResult<CFGEdgeCounterPass>(M);
  Helpers::JsonLLVMHelpers::writeMapToJson(CFGEdgeMap, "CFGEdgeCount");
  return PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getCFGEdgeCounterPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "CFGEdgeCounterPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            // #1 REGISTRATION FOR "opt -passes=print<cfg-edge-count-pass>"
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print<cfg-edge-count-pass>") {
                    MPM.addPass(CFGEdgeCounterPassPrinter(llvm::errs()));
                    return true;
                  }
                  return false;
                });
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "json<cfg-edge-count-pass>") {
                    MPM.addPass(CFGEdgeCounterPassToJson());
                    return true;
                  }
                  return false;
            });
            // #2 REGISTRATION FOR "MAM.getResult<StaticCallCounter>(Module)"
            PB.registerAnalysisRegistrationCallback(
                [](ModuleAnalysisManager &MAM) {
                  MAM.registerPass([&] { return CFGEdgeCounterPass(); });
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getCFGEdgeCounterPassPluginInfo();
}

//------------------------------------------------------------------------------
// Helper functions - implementation
//------------------------------------------------------------------------------
static void printCFGEdgeCounterPassResult(raw_ostream &OutS,
                                     const ResultCFGEdgeCounterPass &CFGEdgekMap) {
  OutS << "================================================="
               << "\n";
  OutS << "LLVM-Practice: CFGEdgeCounterPass results\n";
  OutS << "=================================================\n";
  const char *str1 = "Function Name";
  const char *str2 = "CFG Edge Count";
  OutS << format("%-40s %-10s\n", str1, str2);
  OutS << "-------------------------------------------------"
               << "\n";
  for (auto &edge : CFGEdgekMap) {
    OutS << format("%-40s %-10lu\n", edge.first().str().c_str(),
                           edge.second);
  }
  OutS << "-------------------------------------------------"
               << "\n\n";
}