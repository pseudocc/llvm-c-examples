# include <stdio.h>
# include <string.h>
# include <stdlib.h>

int fib(int n);
int fac(int n);

void print_usage(const char* main, const char* name) {
  printf("\t%s %s {number}\n", main, name);
}

int main(int argc, const char* argv[]) {
  if (argc < 3)
    goto help;
  const char* fn = argv[1];
  const char* an = argv[2];
  int answer;
  if (strcmp(fn, "fac") == 0)
    answer = fac(atoi(an));
  else if (strcmp(fn, "fib") == 0)
    answer = fib(atoi(an));
  else
    goto help;
  printf("%d\n", answer);
  return 0;
help:
  printf("usage:\n");
  print_usage(argv[0], "fac");
  print_usage(argv[0], "fib");
  return 1;
}
