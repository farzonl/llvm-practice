//=============================================================================
// FILE:
//    InstructionCountPass.cpp
//
// DESCRIPTION:
//    Visits all instructions in a function and counts how many times every
//    LLVM IR opcode was used. Prints the output to stderr.
//
//    This example demonstrates how to insert your pass at one of the
//    predefined extension points, e.g. whenever the vectoriser is run (i.e. via
//    `registerVectorizerStartEPCallback` for the new PM).
//
// USAGE:
//    1. New PM
//      opt -load-pass-plugin libInstructionCountPass.dylib `\`
//        -passes="print<inst-count-pass>" `\`
//        -disable-output <input-llvm-file>
//    2. Automatically through an optimisation pipeline - new PM
//      opt -load-pass-plugin libOpcodeCounter.dylib --passes='default<O1>' `\`
//        -disable-output <input-llvm-file>
//
// License: MIT
//=============================================================================
#include "InstructionCountPass.h"
#include "helpers/JsonLLVMHelpers.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

// Pretty-prints the result of this analysis
static void printInstructionCountPassResult(llvm::raw_ostream &,
                              const ResultInstructionCountPass &OC);

//-----------------------------------------------------------------------------
// InstructionCountPass implementation
//-----------------------------------------------------------------------------
llvm::AnalysisKey InstructionCountPass::Key;

InstructionCountPass::Result InstructionCountPass::generateOpcodeMap(llvm::Function &Func) {
  InstructionCountPass::Result OpcodeMap;

  for (auto &BB : Func) {
    for (auto &Inst : BB) {
      StringRef Name = Inst.getOpcodeName();

      if (OpcodeMap.find(Name) == OpcodeMap.end()) {
        OpcodeMap[Inst.getOpcodeName()] = 1;
      } else {
        OpcodeMap[Inst.getOpcodeName()]++;
      }
    }
  }

  return OpcodeMap;
}

InstructionCountPass::Result InstructionCountPass::run(llvm::Function &Func,
                                         llvm::FunctionAnalysisManager &) {
  return generateOpcodeMap(Func);
}

PreservedAnalyses InstructionCountPassPrinter::run(Function &Func,
                                            FunctionAnalysisManager &FAM) {
  auto &OpcodeMap = FAM.getResult<InstructionCountPass>(Func);

  // In the legacy PM, the following string is printed automatically by the
  // pass manager. For the sake of consistency, we're adding this here so that
  // it's also printed when using the new PM.
  OS << "Printing analysis 'InstructionCountPass Pass' for function '"
     << Func.getName() << "':\n";

  printInstructionCountPassResult(OS, OpcodeMap);
  return PreservedAnalyses::all();
}

PreservedAnalyses InstructionCountPassToJson::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &MAM) {
                    
  llvm::FunctionAnalysisManager &FAM =
      MAM.getResult<llvm::FunctionAnalysisManagerModuleProxy>(M).getManager();
  nlohmann::json functions;
  for (auto &Func : M) {
    auto &OpcodeMap = FAM.getResult<InstructionCountPass>(Func);
    functions[Func.getName()] = (Helpers::JsonLLVMHelpers::extractAsJson(OpcodeMap));
  }
  Helpers::JsonLLVMHelpers::writeJsonToFile(functions, "InstructionCount");
  return PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getInstructionCountPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "InstructionCountPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
          // #1 REGISTRATION FOR "opt -passes=print<inst-count-pass>"
          // Register InstructionCountPassPrinter so that it can be used when
          // specifying pass pipelines with `-passes=`.
          PB.registerPipelineParsingCallback(
              [&](StringRef Name, FunctionPassManager &FPM,
                  ArrayRef<PassBuilder::PipelineElement>) {
                if (Name == "print<inst-count-pass>") {
                  FPM.addPass(InstructionCountPassPrinter(llvm::errs()));
                  return true;
                }
                return false;
              });
          // #2 REGISTRATION FOR "opt -passes=json<inst-count-pass>"
          // Register InstructionCountPassToJson so that it can be used when
          // specifying pass pipelines with `-passes=`.
          PB.registerPipelineParsingCallback(
              [&](StringRef Name, ModulePassManager &MPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "json<inst-count-pass>") {
                    MPM.addPass(InstructionCountPassToJson());
                    return true;
                  }
                  return false;
              });
          // #3 REGISTRATION FOR "-O{1|2|3|s}"
          // Register InstructionCountPassPrinter as a step of an existing pipeline.
          // The insertion point is specified by using the
          // 'registerVectorizerStartEPCallback' callback. To be more precise,
          // using this callback means that InstructionCountPassPrinter will be called
          // whenever the vectoriser is used (i.e. when using '-O{1|2|3|s}'.
          PB.registerVectorizerStartEPCallback(
              [](llvm::FunctionPassManager &PM,
                 llvm::OptimizationLevel Level) {
                PM.addPass(InstructionCountPassPrinter(llvm::errs()));
              });
          
          // #4 REGISTRATION FOR "FAM.getResult<InstructionCountPass>(Func)"
          // Register InstructionCountPass as an analysis pass. This is required so that
          // InstructionCountPassPrinter (or any other pass) can request the results
          // of InstructionCountPass.
          PB.registerAnalysisRegistrationCallback(
              [](FunctionAnalysisManager &FAM) {
                FAM.registerPass([&] { return InstructionCountPass(); });
              });
          }
        };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getInstructionCountPassPluginInfo();
}

//------------------------------------------------------------------------------
// Helper functions - implementation
//------------------------------------------------------------------------------
static void printInstructionCountPassResult(raw_ostream &OutS,
                                     const ResultInstructionCountPass &OpcodeMap) {
  OutS << "================================================="
               << "\n";
  OutS << "LLVM-Practice: InstructionCountPass results\n";
  OutS << "=================================================\n";
  const char *str1 = "OPCODE";
  const char *str2 = "#TIMES USED";
  OutS << format("%-20s %-10s\n", str1, str2);
  OutS << "-------------------------------------------------"
               << "\n";
  for (auto &Inst : OpcodeMap) {
    OutS << format("%-20s %-10lu\n", Inst.first().str().c_str(),
                           Inst.second);
  }
  OutS << "-------------------------------------------------"
               << "\n\n";
}