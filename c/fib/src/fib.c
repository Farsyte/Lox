#include <stdio.h>
#include <time.h>
// #include <stdlib.h>

// Worst possible recursive definition of fib,
// to measure the speed of the interpreter and
// later to compare with comiled code.
//
// fib(n) runtime is O(1.618^n)

double fib(int n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}

int main(int argc, char **argv)
{
    (void)argc;			/* intentionally unused */
    (void)argv;			/* intentionally unused */

    double spc = 1000.0 / (double)CLOCKS_PER_SEC;
    for (unsigned i = 0; i <= 64; ++i) {
	clock_t t0 = clock();
	double res = fib(40);
	clock_t tF = clock();
	double dt = (tF - t0) * spc;
	if (0 == (i & (i-1))) {
	    printf("%3d.\t%e\t%.1f ms\n", i, res, dt);
	}
    }

    return 0;
}
