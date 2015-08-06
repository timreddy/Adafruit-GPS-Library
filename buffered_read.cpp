#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "buffered_read.h"

#include "Arduino.h"

BufferedRead::BufferedRead(size_t size) {
  this->buffer_size = size;

  if((this->buffer = (char*) malloc(this->buffer_size)) == NULL) {
    exit(1);
  }

  this->read_ptr = this->write_ptr = this->buffer;
  this->buffer_end = this->buffer + this->buffer_size;

  //
  // initialize to newlines
  //
  for(char* p = this->buffer; p < this->buffer_end; p++) {
    *p = '\n';
  }

}

BufferedRead::~BufferedRead() {
  free(this->buffer);
}

// moving past the end of the buffer will cause the pointer to wrap around silently.
void BufferedRead::move_ptr(char** p, size_t n) {
  size_t offset = (*p - this->buffer + n) % this->buffer_size;
  *p = (char*) (this->buffer + offset);
}

size_t BufferedRead::write_line(char* buffer, size_t max_write_len, char delim) {
  //
  // truncate max write length to the buffer size
  //
  if(max_write_len > this->buffer_size) {
    max_write_len = this->buffer_size;
  }

  //
  // determine the distance to the end of the buffer
  //
  size_t write_len = this->buffer_end - this->write_ptr;
  if(write_len > max_write_len) {
    write_len = max_write_len;
  }

  unsigned int characters_written;
  char* ptr = (char*) memccpy(this->write_ptr, buffer, delim, write_len);

  // 
  // NULL indicates that no delimiter was encountered, and
  // we therefore (may)  need to keep writing to the beginning
  // of the buffer
  //
  if(!ptr) {
    if(max_write_len == write_len) {
      characters_written = write_len;
    } else {
      ptr = (char*) memccpy(this->buffer, buffer + write_len, delim, max_write_len - write_len);
    
      //
      // if again no delimiter was encountered, then we have written to the entire buffer.
      //
      if(!ptr) {
        characters_written = this->buffer_size;
      } else {
        characters_written = ptr - this->buffer + write_len;
      }
    }
  } else {
    characters_written = ptr - this->write_ptr;
  } 

  this->move_ptr(&(this->write_ptr), characters_written);
  return characters_written;
}

size_t BufferedRead::read_line(char* buffer, size_t max_read_len, char delim) {
  //
  // truncate max read length to the buffer size
  //
  if(max_read_len > this->buffer_size) {
    max_read_len = this->buffer_size;
  }

  //
  // determine the distance to the end of the buffer
  //
  size_t read_len = this->buffer_end - this->read_ptr;
  if(read_len > max_read_len) {
    read_len = max_read_len;
  }

  size_t characters_read;
  char* ptr = (char*) memccpy(buffer, this->read_ptr, delim, read_len);

  // 
  // NULL indicates that no delimiter was encountered, and
  // we therefore need to keep reading from the beginning
  // of the buffer
  //
  if(!ptr) {
    ptr = (char*) memccpy(buffer + read_len, this->buffer, delim, max_read_len - read_len);
    
    //
    // if again no delimiter was encountered, then we have read the entire buffer.
    //
    if(!ptr) {
      characters_read = this->buffer_size;
    } else {
      characters_read = ptr - buffer;
    }
  } else {
    characters_read = ptr - buffer;
  } 

  this->move_ptr(&(this->read_ptr), characters_read);
  *(buffer + characters_read) = 0;
  return characters_read;
}

void BufferedRead::print_buffer(void) {
  for(char* p = this->buffer; p < this->buffer + this->buffer_size; p++) {
    if( (size_t)(p - this->buffer) % 40 == 0) {
      printf("\n");
    } else {
      printf("%2x", *p);
    }
  }
}

