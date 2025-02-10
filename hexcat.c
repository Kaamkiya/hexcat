/* SPDX-License-Identifier: MIT */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__unix__) || defined(UNIX) || defined(__linux)
# include <unistd.h>
#endif
#if defined(WIN32) || defined(CYGWIN)
# include <io.h>
#endif


#include "hexcat.h"

#define VERSION 0.1
#define BUFSIZE 16


char
colorize(int ch)
{
	/*
	 * Since space characters are printable, check for them before checking for
	 * other printable characters.
	 */
	if (isspace(ch))
		return 3;
	if (ch == 0)
		return 7;
	if (ch == 255)
		return 4;
	return isprint(ch) ? 2 : 1;
}

void
usage()
{
	fputs("usage: hexcat [-hvncx] [-o FILE] [FILE]\n", stderr);
	exit(0);
}

int
main(int argc, char *argv[])
{
	char *arg = NULL;
	FILE *fp = stdin, *ofp = stdout;

	enum Colorize usecolor = ON;
	int printascii = 1;

	/* Disable colorized output by default for non-TTYs. */
	if (!isatty(STDOUT_FILENO)) usecolor = OFF;

	while (argc >= 2) {
		arg = argv[1] + (!strncmp(argv[1], "--", 2) && argv[1][2]);
		if (!strncmp(arg, "-h", 2)) usage();
		else if (!strncmp(arg, "-n", 2)) usecolor = OFF;
		else if (!strncmp(arg, "-c", 2)) usecolor = FORCE;
		else if (!strncmp(arg, "-x", 2)) printascii = 0;
		else if (!strncmp(arg, "-v", 2)) {
			fprintf(stderr, "hexcat v%.1f\n", VERSION);
			return 0;
		} else if (!strncmp(arg, "-o", 2)) {
			argv++;
			argc--;

			ofp = fopen(argv[1], "w");
			if (ofp == NULL) {
				fprintf(stderr, "hexcat: ");
				perror(argv[1]);
				return 1;
			}

			if (usecolor != FORCE) {
				usecolor = OFF;
			}
		} else break;
		argv++;
		argc--;
	}

	if (argc > 1) {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			fprintf(stderr, "hexcat: ");
			perror(argv[1]);
			return 1;
		}
	}

	unsigned char buf[BUFSIZE];
	unsigned long size = 0;
	size_t bytes_read;

	while ((bytes_read = fread(buf, sizeof(char), BUFSIZE, fp)) > 0) {
		/* Print an 8-character length, 0-padded long int in hexadecimal form. */
		fprintf(ofp, "%08lx: ", size);

		for (int i = 0; i < bytes_read; i++) {
			int c = buf[i];

			if (usecolor)
				fprintf(ofp, "\033[1;3%dm%02x\033[0m ", colorize(c), c);
			else
				fprintf(ofp, "%02x ", buf[i]);

			if (i == 7)
				fprintf(ofp, " ");
		}

		if (bytes_read < 8) fprintf(ofp, " ");

		while (bytes_read < BUFSIZE) {
			fprintf(ofp, "   ");
			buf[bytes_read++] = ' ';
		}

		if (printascii) {
			fprintf(ofp, " | ");

			for (int i = 0; i < BUFSIZE; i++) {
				int c = buf[i];
				if (usecolor)
					fprintf(ofp, "\033[1;3%dm%c\033[0m", colorize(c), isprint(c) ? c : '.');
				else
					fprintf(ofp, "%c", isprint(c) ? c : '.');
			}
		}

		fputc('\n', ofp);

		size += BUFSIZE;

		if (bytes_read < 0)
			perror("Read error.");

		/* Reset all values in the buffer. */
		for (int i = 0; i < BUFSIZE; i++)
			buf[i] = '\0';
	}

	fclose(fp);
	fclose(ofp);
}
