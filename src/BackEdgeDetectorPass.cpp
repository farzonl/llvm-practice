#include "BackEdgeDetectorPass.h"
#include "helpers/JsonLLVMHelpers.h"
#include "helpers/MathHelpers.h"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Analysis/LoopAnalysisManager.h>

using namespace llvm;

// Pretty-prints the result of this analysis
static void printBackEdgeDetectorPassResult(llvm::raw_ostream &,
                              const ResultFunctionBackEdgeCountPair &OC);

llvm::AnalysisKey BackEdgeDetectorPass::Key;

void BackEdgeDetectorPass::getLoopCountNested(llvm::Loop &Toploop, size_t &backEdgesCount) const
{
    std::vector<llvm::Loop *> nestedloops = Toploop.getSubLoops();
    for (auto &nl : nestedloops) 
    {
        backEdgesCount+=nl->getNumBackEdges();
        getLoopCountNested(*nl, backEdgesCount);
    }
}

size_t BackEdgeDetectorPass::getBackEdgeInfo(llvm::LoopInfo &LI) const {
  size_t backEdgesCount = 0;

  for (auto &loop : LI) {
   backEdgesCount+=loop->getNumBackEdges();
   getLoopCountNested(*loop, backEdgesCount);
  }

  return backEdgesCount;
}

BackEdgeDetectorPass::Result BackEdgeDetectorPass::run(llvm::Function &F,
                                         llvm::FunctionAnalysisManager &FAM) {   
  //llvm::LoopAnalysisManager &LAM =
  //    FAM.getResult<llvm::LoopAnalysisManagerFunctionProxy>(F).getManager();                              
  //llvm::LoopInfo &LI = LAM.getResult<llvm::LoopAnalysis>();
  
  llvm::LoopInfo &LI = FAM.getResult<llvm::LoopAnalysis>(F);
  size_t backEdgesCount = /*getBackEdgeInfo(LI);*/ 0;
  
  return BackEdgeDetectorPass::Result(F.getName(), backEdgesCount);
}

PreservedAnalyses BackEdgeDetectorPassPrinter::run(llvm::Function &F,
                                       llvm::FunctionAnalysisManager &FAM) {
                       
  auto functionBackEdgeCount =  FAM.getResult<BackEdgeDetectorPass>(F);
  printBackEdgeDetectorPassResult(OS, functionBackEdgeCount);
  return PreservedAnalyses::all();
}

PreservedAnalyses BackEdgeDetectorPassToJson::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &MAM) {

  llvm::FunctionAnalysisManager &FAM =
      MAM.getResult<llvm::FunctionAnalysisManagerModuleProxy>(M).getManager();
  nlohmann::json functions;
  for (auto &Func : M) {
    auto &pair = FAM.getResult<BackEdgeDetectorPass>(Func);
    functions[Func.getName()] = pair.second;
  }
  Helpers::JsonLLVMHelpers::writeJsonToFile(functions, "BackEdgeCount");
  return PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getBackEdgeDetectorPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "BackEdgeDetectorPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            // #1 REGISTRATION FOR "opt -passes=print<bed-count-pass>"
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, FunctionPassManager &FPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print<bed-count-pass>") {
                    FPM.addPass(BackEdgeDetectorPassPrinter(llvm::errs()));
                    return true;
                  }
                  return false;
                });
            // #2 REGISTRATION FOR "opt -passes=json<bed-count-pass>"
            PB.registerPipelineParsingCallback(
                [&](StringRef Name, ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "json<bed-count-pass>") {
                    MPM.addPass(BackEdgeDetectorPassToJson());
                    return true;
                  }
                  return false;
            });
            // #2 REGISTRATION FOR "-O{1|2|3|s}"
            // Register BackEdgeDetectorPassPrinter as a step of an existing pipeline.
            // The insertion point is specified by using the
            // 'registerVectorizerStartEPCallback' callback. To be more precise,
            // using this callback means that BackEdgeDetectorPassPrinter will be called
            // whenever the vectoriser is used (i.e. when using '-O{1|2|3|s}'.
            PB.registerVectorizerStartEPCallback(
                [](llvm::FunctionPassManager &FPM,
                   llvm::OptimizationLevel Level) {
                  FPM.addPass(BackEdgeDetectorPassPrinter(llvm::errs()));
                });
            // #3 REGISTRATION FOR "FAM.getResult<StaticCallCounter>(Module)"
            PB.registerAnalysisRegistrationCallback(
                [](FunctionAnalysisManager &FAM) {
                  FAM.registerPass([&] { return BackEdgeDetectorPass(); });
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getBackEdgeDetectorPassPluginInfo();
}

//------------------------------------------------------------------------------
// Helper functions - implementation
//------------------------------------------------------------------------------
static void printBackEdgeDetectorPassResult(raw_ostream &OutS,
                                     const ResultFunctionBackEdgeCountPair &EdgeDetectorPair) {
  
  
  OutS << "================================================="
               << "\n";
  OutS << "LLVM-Practice: BackEdgeDetectorPass results\n";
  OutS << "=================================================\n";
  const char *str1 = "Function Name";
  const char *str2 = "Back Edge Count";
  OutS << format("%-40s %-10s\n", str1, str2);
  OutS << "-------------------------------------------------"
       << "\n";
  
  OutS << format("%-40s %-10lu\n", EdgeDetectorPair.first.c_str(),
                           EdgeDetectorPair.second);
  OutS << "-------------------------------------------------"
       << "\n\n";
}