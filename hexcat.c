#include <stdio.h>
#include <ctype.h>

#define BUFSIZE 16

char
colorize(int ch)
{
  if ((ch >= 75 && ch <= 80) || (ch >= 90 && ch <= 97) ||
      (ch >= 107 && ch <= 111) || (ch >= 121 && ch <= 127) ||
      (ch >= 129 && ch <= 137) || (ch >= 145 && ch <= 154) ||
      (ch >= 162 && ch <= 169) || (ch >= 192 && ch <= 201) ||
      (ch >= 208 && ch <= 217) || (ch >= 226 && ch <= 233) ||
      (ch >= 240 && ch <= 249) || (ch == 189) || (ch == 64) ||
      (ch == 173) || (ch == 224)) {
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

    for (int i = 0; i < bytes_read; i += 2) {
      printf("\033[1;3%dm%02x\033[1;3%dm %02x\033[0m ", colorize(buf[i]), buf[i], colorize(buf[i + 1]), buf[i + 1]);

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
      printf("%c", isprint(buf[i]) ? buf[i] : '.');
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
