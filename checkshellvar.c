#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int peek() {
	int c = getchar();
	ungetc(c, stdin);
	return c;
}

int loop(const char *whilech, const char *untilch, int ignoreRule) {
	const static char* validIdCh =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	char single[] = "x";
	int c, p, last, par = 0, brace = 0;
	while ((c = getchar()) != EOF) {
		if (untilch && strchr(untilch, c)
						|| whilech && !strchr(whilech, c))
			return c;
		if (!ignoreRule)
			switch (c) {
			case '\\':
				putchar(c);
				c = getchar();
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
						ungetc(c, stdin);
						continue;
					} else if (p == '{' || p == '(')
						putchar(c = loop(NULL, NULL, FALSE)), c = '"';
					else
						putchar(c = getchar()), c = '"';
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

int main() {
	loop(NULL, NULL, FALSE);
	return 0;
}
