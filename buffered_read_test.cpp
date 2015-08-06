#include "buffered_read.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
  BufferedRead buf(11*4);

  char* line1 = (char*) malloc(82);
  char* line2 = (char*) malloc(82);
  char* line3 = (char*) malloc(82);
  strcpy(line1, "0123456789\n");
  strcpy(line2, "ABCDEFGHIJ\n");
  strcpy(line3, "abcdefghij\n");

  unsigned int n;
  printf("write (%d)\n", (unsigned int) buf.write_line(line1, 82));
  printf("write (%d)\n", (unsigned int) buf.write_line(line2, 82));
  printf("write (%d)\n", (unsigned int) buf.write_line(line3, 82));

  char* read_line = (char*) malloc(82);
  n = buf.read_line(read_line, 82, '\n');
  printf("%d: %s", n, read_line);
  n = buf.read_line(read_line, 82, '\n');
  printf("%d: %s", n, read_line);
  n = buf.read_line(read_line, 82, '\n');
  printf("%d: %s", n, read_line);

  printf("\nMemory dump\n");
  buf.print_buffer();
  printf("\n");

  printf("write (%d)\n", (unsigned int) buf.write_line(line1, 82));
  printf("write (%d)\n", (unsigned int) buf.write_line(line2, 82));
  printf("write (%d)\n", (unsigned int) buf.write_line(line3, 82));
  n = buf.read_line(read_line, 82, '\n');
  printf("%d: %s", n, read_line);
  n = buf.read_line(read_line, 82, '\n');
  printf("%d: %s", n, read_line);
  n = buf.read_line(read_line, 82, '\n');
  printf("%d: %s", n, read_line);
  n = buf.read_line(read_line, 82, '\n');
  printf("%d: %s", n, read_line);
  
  buf.print_buffer();
  printf("\n");
}
