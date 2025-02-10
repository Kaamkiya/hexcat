/* SPDX-License-Identifier: MIT */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "hexcat.h"

char *argv0; /* Required to come before arg.h */
#include "arg.h"

#define BUFSIZE 16

int usecolor = 1;

char
colorize(int ch)
{
	/*
	 * Since space characters are printable, check for them before checking for
	 * other printable characters.
	 */
	if (isspace(ch)) {
		return 3;
	}
	if (ch == 0) {
		return 7;
	}
	if (ch == 255) {
		return 4;
	}
	return isprint(ch) ? 2 : 1;
}

void
usage()
{
	printf("usage: %s [-hv]\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[])
{
	ARGBEGIN {
	case 'h':
		usage();
		return 0;
	case 'v':
		puts("hexcat v0.1");
		return 0;
	} ARGEND;

	FILE *fp = stdin;
	printf("%d\n", usecolor);

	if (argc > 0) {
		/*
		 * argv is consumed by arg.h, so argv0 contains the program name and argv
		 * contains positional arguments.
		 */
		fp = fopen(argv[0], "r");
		if (fp == NULL) {
			perror("Failed to open file.");
			return 1;
		}
	}

	unsigned char buf[BUFSIZE];
	unsigned long size = 0;
	size_t bytes_read;

	while ((bytes_read = fread(buf, sizeof(char), BUFSIZE, fp)) > 0) {
		/* Print an 8-character length, 0-padded long int in hexadecimal form. */
		printf("%08lx: ", size);

		for (int i = 0; i < bytes_read; i++) {
			if (usecolor) {
				printf("\033[1;3%dm%02x\033[0m ", colorize(buf[i]), buf[i]);
			} else {
				printf("%02x ", buf[i]);
			}

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
			printf("\033[1;3%dm%c\033[0m", colorize(c), isprint(c) ? c : '.');
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
