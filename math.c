# include <llvm-c/ExecutionEngine.h>
# include <llvm-c/Target.h>
# include <llvm-c/Analysis.h>
# include <llvm-c/BitWriter.h>

# include <stdio.h>
# include <stdlib.h>
# include "./math.h"

# ifndef MODULE_NAME
# define MODULE_NAME "math"
# endif

static LLVMValueRef define_i32toi32(LLVMModuleRef mod, const char* name) {
  LLVMTypeRef param_types[] = { LLVMInt32Type() };
  // 1 => param_types.length
  // 0 => not a variadic function
  LLVMTypeRef return_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
  LLVMValueRef func = LLVMAddFunction(mod, name, return_type);
  return func;
}

/**
 * int fib(int n) {
 *   int a, b, t;
 *   if (n < 1)
 *     return -1;
 *   if (n < 3)
 *     return 1;
 *   a = 1;
 *   b = 1;
 *   while (n-- > 2) {
 *     t = a + b;
 *     a = b;
 *     b = t;
 *   }
 *   return b;
 * }
 **/

LLVMValueRef define_fib(LLVMModuleRef mod) {
  return define_i32toi32(mod, FIBONACCI);
}

void declare_fib(LLVMValueRef fib) {
  LLVMValueRef c0 = LLVMConstInt(LLVMInt32Type(), 1, 0);
  LLVMValueRef c1 = LLVMConstInt(LLVMInt32Type(), -1, 0);
  LLVMValueRef c2 = LLVMConstInt(LLVMInt32Type(), 3, 0);
  LLVMValueRef c3 = LLVMConstInt(LLVMInt32Type(), 2, 0);
  LLVMValueRef n;
  
  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(fib, "entry");
  LLVMBasicBlockRef err = LLVMAppendBasicBlock(fib, "err");
  LLVMBasicBlockRef noerr = LLVMAppendBasicBlock(fib, "noerr");
  LLVMBasicBlockRef lt3 = LLVMAppendBasicBlock(fib, "lt3");
  LLVMBasicBlockRef body = LLVMAppendBasicBlock(fib, "body");
  LLVMBasicBlockRef loop_cond = LLVMAppendBasicBlock(fib, "loop.cond");
  LLVMBasicBlockRef loop_body = LLVMAppendBasicBlock(fib, "loop.body");
  LLVMBasicBlockRef end = LLVMAppendBasicBlock(fib, "end");

  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);
  LLVMValueRef aptr = LLVMBuildAlloca(builder, LLVMInt32Type(), "aptr");
  LLVMValueRef bptr = LLVMBuildAlloca(builder, LLVMInt32Type(), "bptr");
  LLVMValueRef nptr = LLVMBuildAlloca(builder, LLVMInt32Type(), "nptr");
  LLVMBuildStore(builder, n = LLVMGetParam(fib, 0), nptr);
  LLVMValueRef is_nlt1 = LLVMBuildICmp(builder, LLVMIntSLT, n, c0, "is_nlt1");
  LLVMBuildCondBr(builder, is_nlt1, err, noerr);

  LLVMPositionBuilderAtEnd(builder, err);
  LLVMBuildRet(builder, c1);

  LLVMPositionBuilderAtEnd(builder, noerr);
  LLVMValueRef is_nlt3 = LLVMBuildICmp(builder, LLVMIntSLT, n, c2, "is_nlt3");
  LLVMBuildCondBr(builder, is_nlt3, lt3, body);

  LLVMPositionBuilderAtEnd(builder, lt3);
  LLVMBuildRet(builder, c0);

  LLVMPositionBuilderAtEnd(builder, body);
  LLVMBuildStore(builder, c0, aptr);
  LLVMBuildStore(builder, c0, bptr);
  LLVMBuildBr(builder, loop_cond);

  LLVMPositionBuilderAtEnd(builder, loop_cond);
  n = LLVMBuildLoad(builder, nptr, "n");
  LLVMValueRef is_ngt2 = LLVMBuildICmp(builder, LLVMIntSGT, n, c3, "is_ngt2");
  LLVMValueRef n_minus = LLVMBuildSub(builder, n, c0, "n_minus");
  LLVMBuildStore(builder, n_minus, nptr);
  LLVMBuildCondBr(builder, is_ngt2, loop_body, end);

  LLVMPositionBuilderAtEnd(builder, loop_body);
  LLVMValueRef a = LLVMBuildLoad(builder, aptr, "a");
  LLVMValueRef b = LLVMBuildLoad(builder, bptr, "b");
  LLVMValueRef t = LLVMBuildAdd(builder, a, b, "t");
  LLVMBuildStore(builder, b, aptr);
  LLVMBuildStore(builder, t, bptr);
  LLVMBuildBr(builder, loop_cond);

  LLVMPositionBuilderAtEnd(builder, end);
  LLVMValueRef anwser = LLVMBuildLoad(builder, bptr, "answer");
  LLVMBuildRet(builder, anwser);

  LLVMDisposeBuilder(builder);
}

/**
 * int fac(int n) {
 *   if (n < 0)
 *     return -fac(-n);
 *   switch (n) {
 *   case 0:
 *   case 1:
 *     return 1;
 *   default:
 *     return n * fac(n - 1);
 *   }
 * }
 **/
LLVMValueRef define_fac(LLVMModuleRef mod) {
  return define_i32toi32(mod, FACTORIAL);
}

void declare_fac(LLVMValueRef fac) {
  LLVMValueRef c0 = LLVMConstInt(LLVMInt32Type(), 0, 0);
  LLVMValueRef c1 = LLVMConstInt(LLVMInt32Type(), 1, 0);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(fac, "entry");
  LLVMBasicBlockRef neg = LLVMAppendBasicBlock(fac, "neg");
  LLVMBasicBlockRef match = LLVMAppendBasicBlock(fac, "match");
  LLVMBasicBlockRef is0 = LLVMAppendBasicBlock(fac, "is0");
  LLVMBasicBlockRef is1 = LLVMAppendBasicBlock(fac, "is1");
  LLVMBasicBlockRef gt1 = LLVMAppendBasicBlock(fac, "gt1");
  LLVMBasicBlockRef end = LLVMAppendBasicBlock(fac, "end");
  LLVMBuilderRef builder = LLVMCreateBuilder();

  LLVMPositionBuilderAtEnd(builder, entry);
  LLVMValueRef n = LLVMGetParam(fac, 0);
  LLVMValueRef is_nlt0 = LLVMBuildICmp(builder, LLVMIntSLT, n, c0, "is_nlt0");
  LLVMBuildCondBr(builder, is_nlt0, neg, match);

  LLVMPositionBuilderAtEnd(builder, neg);
  LLVMValueRef absn = LLVMBuildNeg(builder, n, "absn");
  LLVMValueRef abs_fac_args[] = { absn };
  LLVMValueRef abs_fac = LLVMBuildCall(builder, fac, abs_fac_args, 1, "abs_fac");
  LLVMValueRef neg_fac = LLVMBuildNeg(builder, abs_fac, "neg_fac");
  LLVMBuildBr(builder, end);

  LLVMPositionBuilderAtEnd(builder, match);
  LLVMValueRef switch_n = LLVMBuildSwitch(builder, n, gt1, 2);
  LLVMAddCase(switch_n, c0, is0);
  LLVMAddCase(switch_n, c1, is1);

  LLVMPositionBuilderAtEnd(builder, is0);
  LLVMBuildBr(builder, is1);

  LLVMPositionBuilderAtEnd(builder, is1);
  LLVMBuildBr(builder, end);

  LLVMPositionBuilderAtEnd(builder, gt1);
  LLVMValueRef n_minus = LLVMBuildSub(builder, n, c1, "n_minus");
  LLVMValueRef rec_fac_args[] = { n_minus };
  LLVMValueRef rec_fac = LLVMBuildCall(builder, fac, rec_fac_args, 1, "rec_fac");
  LLVMValueRef prod = LLVMBuildMul(builder, n, rec_fac, "prod");
  LLVMBuildBr(builder, end);

  LLVMPositionBuilderAtEnd(builder, end);
  LLVMValueRef answer = LLVMBuildPhi(builder, LLVMInt32Type(), "answer");
  LLVMValueRef phi_vals[] = { neg_fac, c1, prod };
  LLVMBasicBlockRef phi_blocks[] = { neg, is1, gt1 };
  LLVMAddIncoming(answer, phi_vals, phi_blocks, 3);
  LLVMBuildRet(builder, answer);

  LLVMDisposeBuilder(builder);
}

int main(int argc, const char* argv[]) {
  LLVMModuleRef mod;
  LLVMValueRef fib, fac;
  char* error = NULL;
  
  mod = LLVMModuleCreateWithName(MODULE_NAME);
  fib = define_fib(mod);
  fac = define_fac(mod);
  declare_fib(fib);
  declare_fac(fac);

  LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);

  if (LLVMWriteBitcodeToFile(mod, "math.bc") != 0)
    fprintf(stderr, "error writing bitcode to file.\n");

  LLVMDisposeModule(mod);
  
  return 0;
}
