#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"


using namespace llvm;

namespace {

struct SkeletonPass : public PassInfoMixin<SkeletonPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {

        for (auto &F : M) {
            // errs() << "Function body:\n" << F << "\n";
            for (auto& B : F) {
                // errs() << "Basic block:\n" << B << "\n";
                for (auto& I : B) {
                    // errs() << "Instruction:\n" << I << "\n";
                    if (auto* op = dyn_cast<BinaryOperator>(&I)) {
                    // Insert at the point where the instruction `op` appears.
                    IRBuilder<> builder(op);

                    // Make a multiply with the same operands as `op`.
                    Value* lhs = op->getOperand(0);
                    Value* rhs = op->getOperand(1);
                    Value* mul = builder.CreateMul(lhs, rhs);

                    // Everywhere the old instruction was used as an operand, use our
                    // new multiply instruction instead.
                    for (auto& U : op->uses()) {
                        User* user = U.getUser();  // A User is anything with operands.
                        user->setOperand(U.getOperandNo(), mul);
                    }

                    // We modified the code.
                    // return true;
                    }
                }
            }
        }

        return PreservedAnalyses::all();
    }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "Skeleton pass",
        .PluginVersion = "v0.1",
        // .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
        //     PB.registerPipelineStartEPCallback(
        //         [](ModulePassManager &MPM, OptimizationLevel Level) {
        //             MPM.addPass(SkeletonPass());
        //         });
        // }
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            // For opt -passes="skeleton"
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "skeleton") {
                        MPM.addPass(SkeletonPass());
                        return true;
                    }
                    return false;
                });
            
            // For clang -fpass-plugin
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                    MPM.addPass(SkeletonPass());
                });
        }
    };
}