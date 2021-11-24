# ifndef MATH_H
# define MATH_H

# include <llvm-c/Core.h>

# define FIBONACCI "fib"
# define FACTORIAL "fac"

LLVMValueRef define_fib(LLVMModuleRef mod);
void declare_fib(LLVMValueRef fib);

LLVMValueRef define_fac(LLVMModuleRef mod);
void declare_fac(LLVMValueRef fac);

# endif