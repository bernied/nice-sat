/* This code is based on the hash table code found in MiniSat and MiniSat+ */
#ifndef NICESAT_ADTS_HASH_TABLE_H_
#define NICESAT_ADTS_HASH_TABLE_H_

#include <stdint.h>
#include "ADTs/Vec.H"
#include "ADTs/HashFuncs.H"

namespace nicesat {
  // Define some implementations of the Hash/Equality structures
  template<class K> struct Hash  { uint32_t operator()(const K& k) const { return genHash((const char*) &k, sizeof(k)); } };
  template<class K> struct Equal { uint32_t operator()(const K& k1, const K& k2) const { return k1 == k2; } };
  template<class K> struct DeepHash  { uint32_t operator()(const K* k) const { return genHash((const char*) k, sizeof(*k)); } };
  template<class K> struct DeepEqual { uint32_t operator()(const K* k1, const K* k2) const { return *k1 == *k2; } };
  template<> struct Hash<int> { uint32_t operator()(uint32_t k) const { return hashInt32(k); } };

  template<class K, class V, class H = Hash<K>, class E = Equal<K> >
  class HashTable {
    struct Elem { K key; V val; };

    H          hash;
    E          equal;
    
    Vec<Elem>* _table;
    size_t     _size;
    size_t     _cap;

    // Prohibit copying
    HashTable<K, V>& operator= (HashTable<K, V>& other) { assert(0); }
    HashTable                      (HashTable<K, V>& other) { assert(0); }

    void insert_(const K& key, const V& val);
  public:
    HashTable();
    HashTable(size_t initCapacity);
    ~HashTable();

    void clear(bool freeMem = false);
    bool lookup(const K& key, V& val) const;
    bool member(const K& key) const;
    void remove(const K& key);
    void insert (const K& key, const V& val) {
      if ((_size << 1) == _cap) grow(_size + 1); insert_(key, val); ++_size;
    }
    void grow(size_t initSize);

    size_t size() { return _size; }
  };

  template<class K, class V, class H, class E>
  HashTable<K, V, H, E>::HashTable() :
    _table(NULL), _size(0), _cap(0)  {
    grow(0);
  }

  template<class K, class V, class H, class E>
  HashTable<K, V, H, E>::HashTable(size_t initSize) :
    _table(NULL), _size(0), _cap(0) {
    grow(initSize);
  }

  template<class K, class V, class H, class E>
  HashTable<K, V, H, E>::~HashTable() {
    delete[] _table;
  }

  template<class K, class V, class H, class E>
  void HashTable<K, V, H, E>::grow(size_t newSize) {
    const Vec<Elem>* old = _table;
    
    newSize <<= 1;
    if (newSize < _cap) return;
    size_t newCap = bigPrimes[0];
    
    for (size_t i = 1; newCap <= newSize && i < numBigPrimes; i++)
      newCap = bigPrimes[i];

    _table = new Vec<Elem>[newCap];
    for (size_t i = 0; i < _cap; i++)
      for (size_t j = 0; j < old[i].size(); j++)
        insert_(old[i][j].key, old[i][j].val);

    delete[] old;
    _cap = newCap;
  }

  template<class K, class V, class H, class E>
  void HashTable<K, V, H, E>::insert_(const K& k, const V& v) {
    Vec<Elem>& bin = _table[hash(k) % _cap];
    //for (size_t j = 0; j < bin.size(); j++) if (bin[j].key == k) { bin[j].val = v; return; }
    bin.push(); bin.last().key = k; bin.last().val = v;
  }

  template<class K, class V, class H, class E>
  void HashTable<K, V, H, E>::remove(const K& k) {
    Vec<Elem>& bin = _table[hash(k) % _cap];
    size_t j = 0;
    for (; j < bin.size() && bin[j].key != k; j++);
    assert(j < bin.size());
    bin[j] = bin.last();
    bin.pop();
  }

  template<class K, class V, class H, class E>
  void HashTable<K, V, H, E>::clear(bool freeMem) {
    _size = 0;
    if (freeMem) {
      _cap = 0;
      delete[] _table;
      _table = NULL;
      grow(_size);
    }
  }

  template<class K, class V, class H, class E>
  bool HashTable<K, V, H, E>::lookup(const K& key, V& val) const {
    const Vec<Elem>& bin = _table[hash(key) % _cap];
    for (size_t i = 0; i < bin.size(); i++)
      if (equal(bin[i].key, key)) { val = bin[i].val; return true; }
    return false;
  }

  template<class K, class V, class H, class E>
  bool HashTable<K, V, H, E>::member(const K& key) const {
    const Vec<Elem>& bin = _table[hash(key) % _cap];
    for (size_t i = 0; i < bin.size(); i++)
      if (equal(bin[i].key, key)) return true;
    return false;
  }
}

  
#endif//NICESAT_ADTS_HASH_TABLE_H_
