CC=clang-12
CFLAGS=-g `llvm-config-12 --cflags`
LD=clang++-12
LDFLAGS=`llvm-config-12 --cxxflags --ldflags --libs core executionengine interpreter analysis native bitwriter --system-libs`

.PHONY: all
all: math fib fac

math.o: math.c
	$(CC) $(CFLAGS) -c $<

driver.o: driver.c math.bc
	$(CC) $(CFLAGS) -c $<

fac.o: fac.c driver.o
	$(CC) $(CFLAGS) -c $<

fib.o: fib.c driver.o
	$(CC) $(CFLAGS) -c $<

fib: fib.o driver.o
	$(LD) $(LDFLAGS) $^ -o $@

fac: fac.o driver.o
	$(LD) $(LDFLAGS) $^ -o $@

math: math.o
	$(LD) $(LDFLAGS) $< -o $@

math.bc: math
	./math

math.ll: math.bc
	llvm-dis-12 $<

.PHONY: clean
clean:
	rm -f math math.bc math.ll
	rm -f *.o
