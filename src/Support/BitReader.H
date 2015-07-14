#ifndef NICESAT_SUPPORT_BIT_READER_HPP_
#define NICESAT_SUPPORT_BIT_READER_HPP_

#include <istream>
#include <stdint.h>

namespace nicesat {
  class BitReader {
  public:
    BitReader(std::istream& in);
    ~BitReader();
    
    /**
     * Reads k bits from the stream (note that k should be less than or
     * equal to 8 * sizeof(uint32_t).
     * @param k The number of bits to read.
     * @return The bits, interpreted as an integer.
     */
    uint32_t getBits(size_t k);
    
  private:
    /**
     * Helper function for getBits.  Assumes that k is less than or
     * equal to the number of bits remaining in the buffer.
     * @param k The number of bits to read.
     * @return The bits, interpreted as an integer
     */
    uint32_t getBitsH(size_t k);
    
    /**
     * Position in the current buffer.  Measured as the number of
     * remaining unread bits.
     */
    size_t _bitPos;
    
    /**
     * The buffer used for reading.  Should be 4-bytes on every platform.
     */
    uint32_t _buffer;
    
    /**
     * The stream that the information is being read from.
     */
    std::istream& _in;
  };
}
#endif//SUPPORT_BIT_READER_H_
