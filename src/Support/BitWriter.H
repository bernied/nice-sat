#ifndef SUPPORT_BIT_WRITER_HPP_
#define SUPPORT_BIT_WRITER_HPP_

#include <ostream>
#include <stdint.h>

namespace nicesat {
  class BitWriter {
  public:
    BitWriter(std::ostream& out);
    ~BitWriter();
    
    /**
     * Writes the last k bits of n to the stream.  Clearly, k should be
     * less than or equal to 8 * sizeof(uint32_t).
     * @param n The source of the bits to write to the stream
     * @param k The number of bits from n to write to k
     */
    void putBits(uint32_t n, size_t k);
    
    /**
     * Writes sufficient 0s to 'pad' the output (so that it ends
     * cleanly).
     */
    void finish();
  private:
    /**
     * Helper function for putting bits to the stream.  Assumes that k
     * is less than or equal to the number of bits remaining to be put
     * in the buffer.
     * @param n The source of bits to put the stream
     * @param k The number of bits to put to the stream
     */
    void putBitsH(uint32_t n, size_t k);
    
    /**
     * Position in the current buffer.  Measure as the number of
     * remaining unused bits.
     */
    size_t _bitPos;
    
    /**
     * A temporary buffer for the bits being written.  Allows us to
     * assemble the chunks before writing them.
     */
    uint32_t _buffer;
    
    /**
     * The stream that the bits are being written to.
     */
    std::ostream& _out;
  };
}

#endif//SUPPORT_BIT_WRITER_HPP_
