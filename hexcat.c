/* SPDX-License-Identifier: MIT */
#include <stdio.h>
#include <ctype.h>

#define BUFSIZE 16

char
colorize(int ch)
{
	if (isprint(ch)) {
		return 2;
	}

	if (ch == 37 || ch == 13 || ch == 5) {
		return 3;
	}

	if (ch == 0) {
		return 7;
	}
	if (ch == 255) {
		return 4;
	}

	return 1;
}

int
main(int argc, char *argv[])
{
	FILE *fp = stdin;

	if (argc > 1) {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			perror("Failed to open file.");
			return 1;
		}
	}

	unsigned char buf[BUFSIZE];
	unsigned long size = 0;
	size_t bytes_read;

	while ((bytes_read = fread(buf, sizeof(char), BUFSIZE, fp)) > 0) {
		printf("%08lx: ", size);

		for (int i = 0; i < bytes_read; i++) {
			printf("\033[1;3%dm%02x\033[0m ", colorize(buf[i]), buf[i]);

			if (i == 7) {
			 	printf(" ");
			}
		}

		if (bytes_read < 8) {
			printf(" ");
		}

		while (bytes_read < BUFSIZE) {
			printf("   ");
			buf[bytes_read++] = ' ';
		}

		printf(" | ");

		for (int i = 0; i < BUFSIZE; i++) {
			int c = buf[i];
			printf("\033[1;3%dm%c", colorize(c), isprint(c) ? c : '.');
		}
		putchar('\n');

		size += BUFSIZE;

		if (bytes_read < 0) {
			perror("Read error.");
		}

		/* Reset all values in the buffer. */
		for (int i = 0; i < BUFSIZE; i++) {
			buf[i] = '\0';
		}
	}

	fclose(fp);
}
