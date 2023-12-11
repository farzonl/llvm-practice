//==============================================================================
// FILE:
//    BackEdgeDetectorPass.h
//
// DESCRIPTION:
//    Declares the Back Edge Detector Pass:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_PRACTICE_BACK_EDGE_DETECTOR_PASS_H
#define LLVM_PRACTICE_BACK_EDGE_DETECTOR_PASS_H

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
/*using BackEdgeDetectorFunctionMap = llvm::StringMap<int>;
using BackEdgeDetectorMetrics = llvm::StringMap<double>;
using ResultBackEdgeDetectorPairPass = std::pair<BackEdgeDetectorFunctionMap, BackEdgeDetectorMetrics>;*/
using ResultFunctionBackEdgeCountPair = std::pair<std::string, size_t>;

struct BackEdgeDetectorPass : public llvm::AnalysisInfoMixin<BackEdgeDetectorPass> {
  using Result = ResultFunctionBackEdgeCountPair;
  Result run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &);
  void getLoopCountNested(llvm::Loop &Toploop, size_t &backEdgesCount) const;
  size_t getBackEdgeInfo(llvm::LoopInfo &LI) const;
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  // A special type used by analysis passes to provide an address that
  // identifies that particular analysis pass type.
  static llvm::AnalysisKey Key;
  friend struct llvm::AnalysisInfoMixin<BackEdgeDetectorPass>;
};

//------------------------------------------------------------------------------
// New PM interface for the printer pass
//------------------------------------------------------------------------------
class BackEdgeDetectorPassPrinter : public llvm::PassInfoMixin<BackEdgeDetectorPassPrinter> {
public:
  explicit BackEdgeDetectorPassPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  llvm::raw_ostream &OS;
};

class BackEdgeDetectorPassToJson : public llvm::PassInfoMixin<BackEdgeDetectorPassToJson> {
public:
  llvm::PreservedAnalyses run(llvm::Module &F,
                              llvm::ModuleAnalysisManager &FAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
};

#endif // LLVM_PRACTICE_BACK_EDGE_DETECTOR_PASS_H