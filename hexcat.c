/* SPDX-License-Identifier: MIT */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hexcat.h"

#define VERSION 0.1
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
	fputs("usage: hexcat [-hvnc] [FILE]", stderr);
	exit(0);
}

int
main(int argc, char *argv[])
{
	char *pp = NULL;

	while (argc >= 2) {
		pp = argv[1] + (!strncmp(argv[1], "--", 2) && argv[1][2]);
		if (!strncmp(pp, "-h", 2)) usage();
		else if (!strncmp(pp, "-n", 2)) usecolor = 0;
		else if (!strncmp(pp, "-c", 2)) usecolor = 1;
		else if (!strncmp(pp, "-v", 2)) {
			fprintf(stderr, "hexcat v%.1f\n", VERSION);
			return 0;
		} else break;
		argv++;
		argc--;
	}

	FILE *fp = stdin;

	if (argc > 1) {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			fprintf(stderr, "%s: ", argv[1]);
			perror("Failed to open file");
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
			if (usecolor) {
				printf("\033[1;3%dm%c\033[0m", colorize(c), isprint(c) ? c : '.');
			} else {
				printf("%c", isprint(c) ? c : '.');
			}
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
