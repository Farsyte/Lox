#include <stdio.h>

int main(int argc, const char **argv) {
    int ch = getchar();

    if (ch == EOF)
	return 0;

    if (argc > 1) {
	printf("\n");
	for (int argi = 1; argi < argc; ++argi) {
	    printf("%s ", argv[argi]);
	}
	printf("START OF DIFF\n");
    }

    do {
	putchar(ch);
	ch = getchar();
    } while (EOF != ch);

    if (argc > 1) {
	for (int argi = 1; argi < argc; ++argi) {
	    printf("%s ", argv[argi]);
	}
	printf("END OF DIFF, FAILING THE BUILD.\n");
	printf("\n");
    }
    return 1;
}
