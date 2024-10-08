// include llvm APIs
#include "llvm/IR/Function.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include <map>

using namespace llvm;

// anonymous namespaces so that it does not clash with existing modules of llvm
namespace {
struct ComputationalIntensityOpcodeCounterPass
    : public PassInfoMixin<ComputationalIntensityOpcodeCounterPass>,
      public InstVisitor<ComputationalIntensityOpcodeCounterPass> {

  unsigned int ArithmeticOps = 0;
  unsigned int MemoryOps = 0;
  std::map<StringRef, unsigned> OpcodeMap;

  // starting point of my custom pass
  // each function in the test file passes thru this
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    ArithmeticOps = 0;
    MemoryOps = 0;
    OpcodeMap.clear(); // reset for each function
    visit(F);          // traverse each instruction in the function

    double IntensityRatio =
        (MemoryOps == 0) ? ArithmeticOps : (double)ArithmeticOps / MemoryOps;
    double IntensityThreshold = -1.0;

    // output computational intensity analysis
    errs() << "\nFunction '" << F.getName() << "' analysis:\n";
    errs() << "Total Number of  Instructions in this Functions are: "
           << F.getInstructionCount() << '\n';
    errs() << " - Arithmetic Ops: " << ArithmeticOps << "\n";
    errs() << " - Memory Ops: " << MemoryOps << "\n";
    errs() << " - Intensity Ratio: " << IntensityRatio << "\n";

    if (IntensityRatio >= IntensityThreshold) {
      errs() << "Function '" << F.getName()
             << "' has high computational intensity.\n";
    }

    // output opcode count analysis
    errs() << "=================================================\n";
    errs() << "OpcodeCounter results for function '" << F.getName() << "':\n";
    errs() << "=================================================\n";
    const char *Str1 = "OPCODE";
    const char *Str2 = "#TIMES USED";
    errs() << format("%-20s %-10s\n", Str1, Str2);
    errs() << "-------------------------------------------------\n";
    for (auto &Inst : OpcodeMap) {
      errs() << format("%-20s %-10u\n", Inst.first.str().c_str(), Inst.second);
    }
    errs() << "-------------------------------------------------\n\n";

    return PreservedAnalyses::all(); // preserve all original analyses so that
                                     // no modifications are made in the
                                     // original functions
  }

  // visitor method for binary operations (arithmetic operations)
  void visitBinaryOperator(BinaryOperator &I) {
    if (I.getOpcode() == Instruction::Add ||
        I.getOpcode() == Instruction::Sub ||
        I.getOpcode() == Instruction::Mul ||
        I.getOpcode() == Instruction::FAdd ||
        I.getOpcode() == Instruction::FDiv ||
        I.getOpcode() == Instruction::FSub ||
        I.getOpcode() == Instruction::FMul) {
      ArithmeticOps++;
    }

    // track and count all opcodes
    trackOpcode(I);
  }

  // visitor methods for load/store instructions (memory operations)
  void visitLoadInst(LoadInst &I) {
    MemoryOps++;
    trackOpcode(I);
  }

  void visitStoreInst(StoreInst &I) {
    MemoryOps++;
    trackOpcode(I);
  }

  // visitor method for all instructions (fallback for untracked instructions)
  void visitInstruction(Instruction &I) { trackOpcode(I); }

private:
  // helper function to track opcode counts
  void trackOpcode(Instruction &I) {
    StringRef Name = I.getOpcodeName();
    if (OpcodeMap.find(Name) == OpcodeMap.end()) {
      OpcodeMap[Name] = 1;
    } else {
      OpcodeMap[Name]++;
    }
  }
};

} // namespace

// register the pass with the LLVM pass infrastructure
llvm::PassPluginLibraryInfo getComputationalIntensityOpcodeCounterPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ComputationalIntensityOpcodeCounterPass",
          LLVM_VERSION_STRING, [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  // if the tool is invoked with the following flag, start
                  // parsing all functions from the file passed as argument
                  if (Name == "analyze-computational-intensity-opcode") {
                    FPM.addPass(ComputationalIntensityOpcodeCounterPass());
                    return true;
                  }
                  return false;
                });
          }};
}

// export the registered plugin
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getComputationalIntensityOpcodeCounterPluginInfo();
}
