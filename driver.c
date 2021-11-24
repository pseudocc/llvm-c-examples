# include <stdlib.h>
# include "./driver.h"
# include "./math.h"

static LLVMBool driver_init(const char* path) {
  LLVMBool failure;
  LLVMMemoryBufferRef mem;
  char* error;

  if (mod != NULL)
    return 1;

  failure = LLVMCreateMemoryBufferWithContentsOfFile(path, &mem, &error);
  if (failure) {
    fprintf(stderr, "failed to create memory buffer from bitcode file "
      "with error: \n%s\n", error);
    LLVMDisposeMessage(error);
    return failure;
  }

  failure = LLVMParseBitcode(mem, &mod, &error);
  if (failure) {
    fprintf(stderr, "failed to parse bitcode with error: \n%s\n", error);
    LLVMDisposeMessage(error);
    return failure;
  }

  failure = LLVMCreateExecutionEngineForModule(&engine, mod, &error);
  if (failure) {
    fprintf(stderr, "failed to create execution engine with error: \n%s\n", error);
    LLVMDisposeMessage(error);
    LLVMDisposeExecutionEngine(engine);
  }

  return failure;
}

static void driver_dispose() {
  // dispose engine will dispose module too
  // LLVMDisposeModule(mod);
  LLVMDisposeExecutionEngine(engine);

  engine = NULL;
  mod = NULL;
}

static int call_func(const char* name, int n) {
  LLVMValueRef func;
  func = LLVMGetNamedFunction(mod, name);
  LLVMGenericValueRef args[] = { LLVMCreateGenericValueOfInt(LLVMInt32Type(), n, 0) };
  LLVMGenericValueRef answer = LLVMRunFunction(engine, func, 1, args);
  return (int)LLVMGenericValueToInt(answer, 1);
}

typedef int (*itoi_t)(int);

static int run_itoi(const char* name, int argc, const char* argv[]) {
  int n;
  if (argc < 2) {
    fprintf(stderr, "usage: %s n\n", argv[0]);
    return 1;
  }

  if (driver_init("math.bc"))
    exit(EXIT_FAILURE);

  n = atoi(argv[1]);
  printf("fib(%d) = %d\n", n, call_func(name, n));
  driver_dispose();

  return 0;
}

int fib(int argc, const char* argv[]) {
  return run_itoi(FIBONACCI, argc, argv);
}

int fac(int argc, const char* argv[]) {
  return run_itoi(FACTORIAL, argc, argv);
}
