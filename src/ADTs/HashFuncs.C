#include "ADTs/HashFuncs.H"
#include <cstring>
  
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__)    \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

namespace nicesat {
  // 32-bit Mix function found at
  // http://www.cris.com/~ttwang/tech/inthash.htm
  uint32_t hashInt32(uint32_t key) {
    key = ~key + (key << 15);
    key =  key ^ (key >> 12);
    key =  key + (key << 2);
    key =  key ^ (key >> 4);
    key =  key * 2057;
    key =  key ^ (key >> 16);
    return (uint32_t) key;
  }
  
  uint32_t hashInt64(uint64_t key) {
    key = (key << 18) - key - 1; //key = (~key) + (key << 18);
    key = key ^ (key >> 31);
    key = (key + (key << 2)) + (key << 4); // key = key * 21;
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (uint32_t) key;
  }
  
#if SIZEOF_UINT32_T != SIZEOF_UINTPTR_T
  uint32_t hashPtr(uintptr_t ptr) {
    key = (key << 18) - key - 1; //key = (~key) + (key << 18);
    key = key ^ (key >> 31);
    key = (key + (key << 2)) + (key << 4); // key = key * 21;
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);
    return (uint32_t) key;
  }
#else
  uint32_t hashPtr(uintptr_t key) {
    key = ~key + (key << 15);
    key =  key ^ (key >> 12);
    key =  key + (key << 2);
    key =  key ^ (key >> 4);
    key =  key * 2057;
    key =  key ^ (key >> 16);
    return key;
  }
#endif
    
#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)  \
                      +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif
  
  uint32_t genHash (const char * data, size_t len, uint32_t hash) {
    uint32_t tmp;
    int rem;
    
    if (len <= 0 || data == NULL) return 0;
    
    rem = len & 3;
    len >>= 2;
  
    /* Main loop */
    for (;len > 0; len--) {
      hash  += get16bits (data);
      tmp    = (get16bits (data+2) << 11) ^ hash;
      hash   = (hash << 16) ^ tmp;
      data  += 2*sizeof (uint16_t);
      hash  += hash >> 11;
    }
    
    /* Handle end cases */
    switch (rem) {
      case 3: hash += get16bits (data);
        hash ^= hash << 16;
        hash ^= data[sizeof (uint16_t)] << 18;
        hash += hash >> 11;
        break;
      case 2: hash += get16bits (data);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
      case 1: hash += *data;
        hash ^= hash << 10;
        hash += hash >> 1;
    }
    
    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
  
    return hash;
  }
}
