CC=clang-12
CFLAGS=-g `llvm-config-12 --cflags`
LD=clang++-12
LDFLAGS=`llvm-config-12 --cxxflags --ldflags --libs core executionengine interpreter analysis native bitwriter --system-libs`

all: fib

fib.o: fib.c
	$(CC) $(CFLAGS) -c $<

fib: fib.o
	$(LD) $(LDFLAGS) $< -o $@

fib.bc: fib
	./fib 0

fib.ll: fib.bc
	llvm-dis-12 $<

clean:
	rm -f fib.o fib fib.bc fib.ll
