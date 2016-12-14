#include <stdio.h>
#include <ctype.h>
#include <string.h>

FILE *fin;

#define TRUE 1
#define FALSE 0

int peek() {
	int c = fgetc(fin);
	ungetc(c, fin);
	return c;
}

int loop(const char *whilech, const char *untilch, int ignoreRule) {
	const static char* validIdCh =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
	char single[] = "x";
	int c, p, last=' ', par = 0, brace = 0;
	while ((c = fgetc(fin)) != EOF) {
		if (untilch && strchr(untilch, c) || whilech && !strchr(whilech, c))
			return c;
		if (!ignoreRule)
			switch (c) {
			case '\\':
				putchar(c);
				c = fgetc(fin);
				break;
			case '"':
			case '\'':
				*single = c;
				putchar(c);
				c = loop(NULL, single, TRUE);
				break;
			case '#':
				if (isspace(last)) {
					*single = '\n';
					putchar(c);
					c = loop(NULL, single, TRUE);
				}
				break;
			case '`':
				*single = '`';
				printf("\"$(");
				c = loop(NULL, single, FALSE);
				putchar(')');
				c = '"';
				break;
			case '$':
				printf("\"$");
				p = peek();
				if (isalnum(p)) {
					c = loop(validIdCh, NULL, TRUE);
					putchar('"');
					ungetc(c, fin);
					continue;
				} else if (p == '{' || p == '(')
					putchar(c = loop(NULL, NULL, FALSE)), c = '"';
				else
					putchar(c = fgetc(fin)), c = '"';
				break;
			case '{':
				brace++;
				break;
			case '}':
				if (!--brace && !par)
					return c;
				break;
			case '(':
				par++;
				break;
			case ')':
				if (!--par && !brace)
					return c;
				break;
			}
		putchar(c);
		last = c;
	}
	return c;
}

int main(int argc, char *argv[]) {
	if (argc == 1)
		fin = stdin;
	else if (!(fin = fopen(argv[1], "rt"))) {
		fprintf(stderr, "Cannot open '%s'", argv[1]);
		return 1;
	}

	loop(NULL, NULL, FALSE);

	if (fin != stdin)
		fclose(fin);
	return 0;
}
