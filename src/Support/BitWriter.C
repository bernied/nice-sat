#include "Support/BitWriter.H"

#include <arpa/inet.h>
#include <cassert>

namespace nicesat {
  static const size_t numBits = sizeof(uint32_t) * 8;
  
#define BIT_MASK(n) ((uint32_t) ((1 << (n)) - 1))
  
  BitWriter::BitWriter(std::ostream& out) : _bitPos(numBits), _buffer(0), _out(out) {
    // Intentionally Empty
  }
  
  BitWriter::~BitWriter() {
    // Intentionally Empty
  }
  
  void BitWriter::putBits(uint32_t n, size_t k) {
    assert(k <= numBits);
    if (k > _bitPos) {
      k -= _bitPos;
      putBitsH(n >> k, _bitPos);
    }
    putBitsH(n, k);
  }
  
  void BitWriter::putBitsH(uint32_t n, size_t k) {
    assert(k <= _bitPos);
    _bitPos -= k;
    _buffer |= (n & BIT_MASK(k)) << _bitPos;
    
    if (_bitPos == 0) {
      _buffer = htonl(_buffer);
      _out.write((char*) &_buffer, sizeof(_buffer));
      _buffer = 0;
      _bitPos = numBits;
    }
  }
  
  void BitWriter::finish() {
    putBitsH(0, _bitPos); // align ending
  }
}
