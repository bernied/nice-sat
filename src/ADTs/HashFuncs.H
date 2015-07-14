#ifndef NICESAT_ADTS_HASH_FUNCS_H_
#define NICESAT_ADTS_HASH_FUNCS_H_

#include <stdint.h>
#include <stdlib.h>

namespace nicesat {
  static const size_t bigPrimes[] = {
    1543, 3079, 6151,
    12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869,
    3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
  };
  
  static const size_t numBigPrimes =
    sizeof(bigPrimes) / sizeof(bigPrimes[0]);
  
  static inline void hashCombine(uint32_t& acc, uint32_t other) {
    acc ^= other + 0x9e3779b9 + (acc << 6) + (acc >> 2);
  }

  uint32_t hashInt32(uint32_t key);
  uint32_t hashInt64(uint64_t key);
  uint32_t hashPtr(uintptr_t ptr);
  uint32_t genHash(const char* data, size_t len, uint32_t hash = 0);
}
  
#endif//NICESAT_ADTS_HASH_FUNCS_H_
