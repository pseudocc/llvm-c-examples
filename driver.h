# ifndef DRIVER_H
# define DRIVER_H

# include <llvm-c/ExecutionEngine.h>
# include <llvm-c/BitReader.h>
# include <llvm-c/Core.h>

# include <stdio.h>

static LLVMModuleRef mod = NULL;
static LLVMExecutionEngineRef engine = NULL;

static LLVMBool driver_init(const char* path);
static void driver_dispose();

int fib(int argc, const char* argv[]);
int fac(int argc, const char* argv[]);

# endif