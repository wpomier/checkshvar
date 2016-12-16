#include <stdio.h>
#include <ctype.h>
#include <string.h>

FILE *fin;

#define NORMAL			0x000
#define IGNORE_RULE 	0x100
#define EXIT_WHEN_CLOSE	0x200

int peek() {
	int c = fgetc(fin);
	ungetc(c, fin);
	return c;
}

int loop(const char *whilech, const char *untilch, int action) {
	const static char* validIdCh =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
	char single[] = "x";
	int c, p, last = ' ', par = 0, brace = 0;
	while ((c = fgetc(fin)) != EOF) {
		if (untilch && strchr(untilch, c) || whilech && !strchr(whilech, c))
			return c;
		if (c == '\\')
			putchar(c), c = fgetc(fin);
		else 
		if (action != IGNORE_RULE)
			switch (c) {
			case '"':
			case '\'':
				*single = c;
				putchar(c);
				c = loop(NULL, single, IGNORE_RULE);
				break;
			case '#':
				if (isspace(last)) {
					*single = '\n';
					putchar(c);
					c = loop(NULL, single, IGNORE_RULE);
				}
				break;
			case '`':
				*single = '`';
				printf("\"$(");
				c = loop(NULL, single, NORMAL);
				putchar(')');
				c = '"';
				break;
			case '$':
				printf("\"$");
				p = peek();
				if (isalnum(p)) {
					c = loop(validIdCh, NULL, IGNORE_RULE);
					putchar('"');
					ungetc(c, fin);
					continue;
				} else if (p == '{' || p == '(')
					putchar(c = loop(NULL, NULL, EXIT_WHEN_CLOSE)), c = '"';
				else
					putchar(c = fgetc(fin)), c = '"';
				break;
			case '{':
				brace++;
				break;
			case '}':
				if (!--brace && !par && action == EXIT_WHEN_CLOSE)
					return c;
				break;
			case '(':
				par++;
				break;
			case ')':
				if (!--par && !brace && action == EXIT_WHEN_CLOSE)
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

	loop(NULL, NULL, NORMAL);

	if (fin != stdin)
		fclose(fin);
	return 0;
}
