# LLVM-C EXAMPLE

works well on ubuntu 20.04.

## Installation

LLVM and Clang.

```bash
sudo apt install llvm-12
sudo apt install clang-12
```

## Build

with GNU Make.

```bash
make
```

## Run from LLVM IR Bitcode

Run fac.

```bash
./fac 8
# fac(8) = 40320
```

Run fib.

```bash
./fib 13
# fib(13) = 233
```

## LLVM IR Text Format

```bash
make math.ll
cat math.ll
```

## Fibonacci

this contains basic operations on parameters and local variables.
also if statement and while loop for control flow.

0. compare
0. loop
0. assignment
0. arithmetic

```c
int fib(int n) {
  int a, b, t;
  if (n < 1)
    return -1;
  if (n < 3)
    return 1;
  a = 1;
  b = 1;
  while (n-- > 2) {
    t = a + b;
    a = b;
    b = t;
  }
  return b;
}
```

## Factorial

this contains recursive call and if statement and switch block.
using a PHI instruction to catch all return value of each block.

0. compare
0. function call
0. arithmetic
0. switch
0. PHI instruction

```c
int fac(int n) {
  if (n < 0)
    return -fac(-n);
  switch (n) {
  case 0:
  case 1:
    return 1;
  default:
    return n * fac(n - 1);
  }
}
```