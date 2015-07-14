#include "Support/BitReader.H"

#include <arpa/inet.h>
#include <cassert>

namespace nicesat {
  static const size_t numBits = sizeof(uint32_t) * 8;
  
#define BIT_MASK(n) ((uint32_t) ((1 << (n)) - 1))
  
  BitReader::BitReader(std::istream& in) : _bitPos(0), _in(in) {
    // Intentionally Empty
  }
  
  BitReader::~BitReader() {
    // Intentionally Empty
  }
  
  uint32_t BitReader::getBits(size_t k) {
    assert(k <= numBits);
    uint32_t ans = 0;
    if (k > _bitPos) {
      k -= _bitPos;
      ans = getBitsH(_bitPos) << k;
    }
    return ans | getBitsH(k);
  }
  
  uint32_t BitReader::getBitsH(size_t k) {
    if (_bitPos == 0) {
      _in.read((char*) &_buffer, sizeof(_buffer));
      _buffer = ntohl(_buffer);
      _bitPos = numBits;
    }
    assert(k <= _bitPos);
    _bitPos -= k;
    return (_buffer >> _bitPos) & BIT_MASK(k);
  }
}
