//==============================================================================
// FILE:
//    CFGEdgeCounterPass.h
//
// DESCRIPTION:
//    Declares the CFG Edge Counter Pass:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_PRACTICE_CFG_EDGE_COUNTER_PASS_H
#define LLVM_PRACTICE_CFG_EDGE_COUNTER_PASS_H

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
using ResultCFGEdgeCounterPass = llvm::StringMap<unsigned>;

struct CFGEdgeCounterPass : public llvm::AnalysisInfoMixin<CFGEdgeCounterPass> {
  using Result = ResultCFGEdgeCounterPass;
  Result run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &);
  CFGEdgeCounterPass::Result getCFGEdgeInfo(const llvm::Module& M) const;
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  // A special type used by analysis passes to provide an address that
  // identifies that particular analysis pass type.
  static llvm::AnalysisKey Key;
  friend struct llvm::AnalysisInfoMixin<CFGEdgeCounterPass>;
};

//------------------------------------------------------------------------------
// New PM interface for the printer pass
//------------------------------------------------------------------------------
class CFGEdgeCounterPassPrinter : public llvm::PassInfoMixin<CFGEdgeCounterPassPrinter> {
public:
  explicit CFGEdgeCounterPassPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  llvm::raw_ostream &OS;
};

class CFGEdgeCounterPassToJson : public llvm::PassInfoMixin<CFGEdgeCounterPassToJson> {
public:
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
};

#endif // LLVM_PRACTICE_CFG_EDGE_COUNTER_PASS_H