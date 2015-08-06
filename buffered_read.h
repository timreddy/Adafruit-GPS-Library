#include <stdlib.h>

#ifndef BUFFERED_READ_H
#define BUFFERED_READ_H

class BufferedRead {
  private: 
    char* buffer;
    char* buffer_end; 
    char* read_ptr;
    char* write_ptr;

    size_t buffer_size;

    void move_ptr(char** p, size_t n);

  public:
    BufferedRead(size_t size);
    ~BufferedRead();

    size_t read_line(char* buffer, size_t max_read_len, char delim = '\n');
    size_t write_line(char* buffer, size_t max_write_len, char delim = '\n');

    void print_buffer(void);
};

#endif
