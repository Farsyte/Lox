#include <stdio.h>

int main(int argc, const char **argv) {
    int ch = getchar();
    int och = '\n';
    if (ch == EOF) return 0;
    if (argc > 1) {
	for (int argi = 1; argi < argc; ++argi) {
	    printf("%s ", argv[argi]);
	}
	printf("START OF DIFF\n");
    }
    for (;;) {
	if (och == '\n')
	    printf("=====  ");
	putchar(ch);
	och = ch;
	ch = getchar();
	if (ch == EOF)
	    break;
    }
    if (argc > 1) {
	for (int argi = 1; argi < argc; ++argi) {
	    printf("%s ", argv[argi]);
	}
	printf("END OF DIFF, FAILING THE BUILD.\n");
    }
    return 1;
}
