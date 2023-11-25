//==============================================================================
// FILE:
//    BasicBlockCounterPass.h
//
// DESCRIPTION:
//    Declares the Basic Block Counter Pass:
//      * new pass manager interface
//      * printer pass for the new pass manager
//
// License: MIT
//==============================================================================
#ifndef LLVM_PRACTICE_BASIC_BLOCK_COUNTER_PASS_H
#define LLVM_PRACTICE_BASIC_BLOCK_COUNTER_PASS_H

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
using BasicBlockFunctionMap = llvm::StringMap<int>;
using BasicBlockMetrics = llvm::StringMap<double>;
using ResultBasicBlockPairPass = std::pair<BasicBlockFunctionMap, BasicBlockMetrics>;

struct BasicBlockCounterPass : public llvm::AnalysisInfoMixin<BasicBlockCounterPass> {
  using Result = ResultBasicBlockPairPass;
  Result run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &);
  BasicBlockCounterPass::Result getBasicBlockInfo(const llvm::Module& M) const;
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  // A special type used by analysis passes to provide an address that
  // identifies that particular analysis pass type.
  static llvm::AnalysisKey Key;
  friend struct llvm::AnalysisInfoMixin<BasicBlockCounterPass>;
};

//------------------------------------------------------------------------------
// New PM interface for the printer pass
//------------------------------------------------------------------------------
class BasicBlockCounterPassPrinter : public llvm::PassInfoMixin<BasicBlockCounterPassPrinter> {
public:
  explicit BasicBlockCounterPassPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  llvm::raw_ostream &OS;
};

class BasicBlockCounterPassToJson : public llvm::PassInfoMixin<BasicBlockCounterPassToJson> {
public:
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
};

#endif // LLVM_PRACTICE_BASIC_BLOCK_COUNTER_PASS_H