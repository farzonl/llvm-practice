//==============================================================================
// FILE:
//    InstructionCountPass.h
//
// DESCRIPTION:
//    Declares The Instruction Count Pass:
//      * uses the new pass manager interface
//      * printer pass for the new pass manager
//      * can export to json
//
// License: MIT
//==============================================================================
#ifndef LLVM_PRACTICE_SIMPLE_PASS_H
#define LLVM_PRACTICE_SIMPLE_PASS_H

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
using ResultInstructionCountPass = llvm::StringMap<unsigned>;

struct InstructionCountPass : public llvm::AnalysisInfoMixin<InstructionCountPass> {
  using Result = ResultInstructionCountPass;
  Result run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &);

  InstructionCountPass::Result generateOpcodeMap(llvm::Function &F);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  // A special type used by analysis passes to provide an address that
  // identifies that particular analysis pass type.
  static llvm::AnalysisKey Key;
  friend struct llvm::AnalysisInfoMixin<InstructionCountPass>;
};

//------------------------------------------------------------------------------
// New PM interface for the printer pass
//------------------------------------------------------------------------------
class InstructionCountPassPrinter : public llvm::PassInfoMixin<InstructionCountPassPrinter> {
public:
  explicit InstructionCountPassPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
  llvm::PreservedAnalyses run(llvm::Function &Func,
                              llvm::FunctionAnalysisManager &FAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
  llvm::raw_ostream &OS;
};

class InstructionCountPassToJson : public llvm::PassInfoMixin<InstructionCountPassToJson> {
public:
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
  // Part of the official API:
  //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
  static bool isRequired() { return true; }

private:
};

#endif