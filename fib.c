# include <llvm-c/Core.h>
# include <llvm-c/ExecutionEngine.h>
# include <llvm-c/Target.h>
# include <llvm-c/Analysis.h>
# include <llvm-c/BitWriter.h>

# include <inttypes.h>
# include <stdio.h>
# include <stdlib.h>

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

# ifndef MODULE_NAME
# define MODULE_NAME "math"
# endif

LLVMValueRef define_fib(LLVMModuleRef mod) {
  LLVMTypeRef param_types[] = { LLVMInt32Type() };
  // 1 => param_types.length
  // 0 => not a variadic function
  LLVMTypeRef return_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
  LLVMValueRef fib = LLVMAddFunction(mod, "fib", return_type);
  return fib;
}

LLVMBuilderRef declare_fib(LLVMValueRef fib) {
  LLVMValueRef c0 = LLVMConstInt(LLVMInt32Type(), 1, 0);
  LLVMValueRef c1 = LLVMConstInt(LLVMInt32Type(), -1, 0);
  LLVMValueRef c2 = LLVMConstInt(LLVMInt32Type(), 3, 0);
  LLVMValueRef c3 = LLVMConstInt(LLVMInt32Type(), 2, 0);
  
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
  LLVMBuildStore(builder, LLVMGetParam(fib, 0), nptr);
  LLVMValueRef n = LLVMBuildLoad(builder, nptr, "n");
  LLVMValueRef is_nlt1 = LLVMBuildICmp(builder, LLVMIntSLT, n, c0, "n < 1");
  LLVMBuildCondBr(builder, is_nlt1, err, noerr);

  LLVMPositionBuilderAtEnd(builder, err);
  LLVMBuildRet(builder, c1);

  LLVMPositionBuilderAtEnd(builder, noerr);
  LLVMValueRef is_nlt3 = LLVMBuildICmp(builder, LLVMIntSLT, n, c2, "n < 3");
  LLVMBuildCondBr(builder, is_nlt3, lt3, body);

  LLVMPositionBuilderAtEnd(builder, lt3);
  LLVMBuildRet(builder, c0);

  LLVMPositionBuilderAtEnd(builder, body);
  LLVMBuildStore(builder, c0, aptr);
  LLVMBuildStore(builder, c0, bptr);
  LLVMBuildBr(builder, loop_cond);

  LLVMPositionBuilderAtEnd(builder, loop_cond);
  LLVMValueRef is_true = LLVMBuildICmp(builder, LLVMIntSGT, n, c3, "n-- > 2");
  LLVMValueRef n_minus = LLVMBuildSub(builder, n, c0, "n_minus");
  LLVMBuildStore(builder, n_minus, nptr);
  LLVMBuildCondBr(builder, is_true, loop_body, end);

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

  return builder;
}

int main(int argc, const char* argv[]) {
  LLVMModuleRef mod;
  LLVMValueRef fib;
  LLVMExecutionEngineRef engine;
  LLVMBuilderRef builder;
  char* error = NULL;
  
  mod = LLVMModuleCreateWithName(MODULE_NAME);
  fib = define_fib(mod);
  builder = declare_fib(fib);

  LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);

  error = NULL;
  if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
    fprintf(stderr, "failed to create execution engine with error: \n%s\n", error);
    abort();
  }
  if (error) {
    LLVMDisposeMessage(error);
    exit(EXIT_FAILURE);
  }

  if (argc < 2) {
    fprintf(stderr, "usage: %s n\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int n = atoi(argv[1]);

  LLVMGenericValueRef args[] = { LLVMCreateGenericValueOfInt(LLVMInt32Type(), n, 0) };
  LLVMGenericValueRef anwser = LLVMRunFunction(engine, fib, 1, args);
  printf("%d\n", (int)LLVMGenericValueToInt(anwser, 0));

  // Write out bitcode to file
  if (LLVMWriteBitcodeToFile(mod, "fib.bc") != 0)
    fprintf(stderr, "error writing bitcode to file, skipping\n");

  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
  
  return 0;
}
