#include "Support/Reader.H"
#include "Support/Shared.H"

#include <string.h>

#include <cmath>

namespace nicesat {
  Reader::Reader(const char* filename) :
    _in(fopen(filename, "r")), _bufPos(0), _bufLen(0) {
    _filename = (char*) malloc(strlen(filename) + 1);
    strcpy(_filename, filename);
    if (_in == NULL) {
      errorf("Unable to open file '%s' for reading", filename);
    }
  }
  
  Reader::~Reader() {
    fclose(_in);
    free(_filename);
  }

  void Reader::fillBuff() {
    _bufLen = fread(_buffer, sizeof(char), READER_BUFFER_LEN, _in);
    if (_bufLen == 0) {
      errorf("fread failed while reading '%s'", _filename);
    }
    _bufPos = 0;
  }

  unsigned char Reader::getNonEofChar() {
    if (_bufPos >= _bufLen) {
      fillBuff();
    }
    return _buffer[_bufPos++];
  }

  static inline int combine(int x, int y) {
    int tmp = y;
    while (tmp != 0) x *= 10, tmp /= 10;
    return x + y;
  }

  int Reader::readNum(char c) {
    char seps[] = { c, '\0'};
    return readNum(seps);
  }
  
  int Reader::readNum(const char* seps) {
    if (_bufPos >= _bufLen) {
      fillBuff();
    }
    
    int n = 0;
    char* start = _buffer + _bufPos;
    while (strchr(seps, _buffer[_bufPos]) == NULL) {
      ++_bufPos;
      if (_bufPos >= _bufLen) {
        _buffer[_bufPos++] = '\0';
        int newN = atoi(start);
        start = _buffer;
        n = combine(n, newN);
        fillBuff();
      }
    }
    
    /* while (strchr(seps, _buffer[_bufPos]) != NULL) {
      _buffer[_bufPos++] = '\0';
      }*/

    if (strchr(seps, _buffer[_bufPos]) != NULL)  _buffer[_bufPos++] = '\0';
    
    int newN = atoi(start);
    return combine(n, newN);
  }
  
  void Reader::readExpected(const char* str) {
    if (_bufPos >= _bufLen) {
      fillBuff();
    }
    
    const char* c = str;
    while (*c != '\0') {
    if (_buffer[_bufPos++] != *c++) {
      errorf("Unexpected character while reading '%s'.  Expecting '%s'.",
             _filename, str);
    }
  }
}

  std::string Reader::readString(char sep) {
    char seps[] = { sep, '\n', '\0' };
    return readString(seps);
  }
  
  std::string Reader::readString(const char* seps) {
    if (_bufPos >= _bufLen) {
      fillBuff();
    }
    
    const char* start = _buffer + _bufPos;
    std::string ans;
    while (strchr(seps, _buffer[_bufPos]) == NULL) {
      ++_bufPos;
      if (_bufPos >= _bufLen) {
        _buffer[READER_BUFFER_LEN] = '\0';
        ans += std::string(start);
        start = _buffer;
        fillBuff();
      }
    }
    
    while (_bufPos < _bufLen && strchr(seps, _buffer[_bufPos]) != NULL) {
      _buffer[_bufPos++] = '\0';
    }
    return ans + std::string(start);
  }
  
  unsigned int Reader::readNumBinary() {
    unsigned int x = 0;
    unsigned int i = 0;
    unsigned char ch;
    
    while ((ch = getNonEofChar()) & 0x80) {
      x |= (ch & 0x7f) << (7 * i++);
    }
    return x | (ch << (7 * i));
  }
}
