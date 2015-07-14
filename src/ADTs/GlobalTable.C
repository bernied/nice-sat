#include <cstring>
#include "ADTs/GlobalTable.H"

namespace nicesat {
  void GlobalTable::clear(bool freeMem) {
    for (size_t i = 0; i < _cap; i++) {
      if (!_table[i].key.isNull() && _table[i].key.isNode()) delete (Node*) _table[i].key.getNodePtr();
      _table[i].key = NULL;
    }

    
    if (freeMem) {
      delete[] _table;
      _table = NULL;
      _maxSize = _cap = 0;
    }
    
    _size = 0;
  }

  void GlobalTable::grow(size_t numElems) {
    HashEntry* old = _table;

    // Find the new capacity
    numElems += 2; numElems <<= 1; // make some extra room (want maxSize > numElems)
    if (numElems < _cap) return;
    size_t newCap = bigPrimes[0];
    for (size_t i = 1; newCap <= numElems && i < numBigPrimes; i++) {
      newCap = bigPrimes[i];
    }

    // Allocate new table
    _table = new HashEntry[newCap];
    memset(_table, 0, sizeof(HashEntry) * newCap);

    // Fill it
    if (old != NULL) {
      for (size_t i = 0; i < _cap; i++) {
        if (!old[i].key.isNull()) {
          size_t newI = old[i].hashVal % newCap;
          size_t incr = 1;
          while (!_table[newI].key.isNull()) { newI = (newI + incr) % newCap; incr += 2; }
          _table[newI].hashVal = old[i].hashVal;
          _table[newI].key = old[i].key;
        }
      }
    }

    // Update values
    delete[] old;
    _cap = newCap;
    _maxSize = (_cap >> 1) - 1;
  }

  bool GlobalTable::lookup(uint32_t hashVal, NodeOp op, const Vec<Edge>& args, size_t& index) {
    size_t incr = 1;
    while (!_table[index].key.isNull()) {
      if (_table[index].hashVal == hashVal) {
        if (_table[index].key->op() == op && _table[index].key->size() == args.size()) {
          bool match = true;
          for (size_t i = 0; i < args.size(); i++)
            if (args[i] != (*(_table[index]).key)[i]) {
              match = false;
              break;
            }
          
          if (match == true) return true;
        }
      }
      index = (index + incr) % _cap;
      incr += 2;
    }
    return false;
  }

  bool GlobalTable::lookup(uint32_t hashVal, Edge e, size_t& index) {
    size_t incr = 1;
    while (!_table[index].key.isNull()) {
      if (_table[index].hashVal == hashVal) {
        if (_table[index].key->op() == e->op() && _table[index].key->size() == e->size()) {
          bool match = true;
          for (size_t i = 0; i < e->size(); i++)
            if ((*e)[i] != (*_table[index].key)[i]) {
              match = false;
              break;
            }
          
          if (match == true) return true;
        }
      }
      index = (index + incr) % _cap;
      incr += 2;
    }
    return false;
  }
  
  bool GlobalTable::lookup(uint32_t hashVal, NodeOp op, Edge arg0, Edge arg1, size_t& index) {
    size_t incr = 1;
    while (!_table[index].key.isNull()) {
      if (_table[index].hashVal == hashVal && _table[index].key->op() == op &&
          _table[index].key->size() == 2   && (*(_table[index].key))[0] == arg0 &&
          (*(_table[index].key))[1] == arg1)
        return true;
      index = (index + incr) % _cap;
      incr += 2;
    }
    return false;
  }

  bool GlobalTable::lookup(uint32_t hashVal, NodeOp op, Edge arg0, Edge arg1, Edge arg2, size_t& index) {
    size_t incr = 1;
    while (!_table[index].key.isNull()) {
      if (_table[index].hashVal == hashVal && _table[index].key->op() == op &&
          _table[index].key->size() == 3   && (*(_table[index].key))[0] == arg0 &&
          (*(_table[index]).key)[1] == arg1  && (*(_table[index].key))[2] == arg2)
        return true;
      index = (index + incr) % _cap;
      incr += 2;
    }
    return false;
  }

  Edge GlobalTable::uNode(NodeOp op, const Vec<Edge>& args) {
    if (_size == _maxSize) grow(_size + 1);
    assert(_size < _maxSize);
    
    uint32_t hashVal = hashInt32(op);
    hashCombine(hashVal, hashInt32(args.size()));
    for (size_t i = 0; i < args.size(); i++) hashCombine(hashVal, hashPtr(args[i]));

    size_t index = hashVal % _cap;
    if (!lookup(hashVal, op, args, index)) {
      _table[index].key     = Edge(new (args.size()) Node(op, args));
      _table[index].hashVal = hashVal;
      ++_size;
    }

    return _table[index].key;
  }
  
  Edge GlobalTable::uNode(NodeOp op, Edge arg0, Edge arg1) {
    if (_size == _maxSize) grow(_size + 1);
    assert(_size < _maxSize);

    uint32_t hashVal = hashInt32(op);
    hashCombine(hashVal, hashInt32(2));
    hashCombine(hashVal, hashPtr(arg0));
    hashCombine(hashVal, hashPtr(arg1));

    size_t index = hashVal % _cap;
    if (!lookup(hashVal, op, arg0, arg1, index)) {
      _table[index].key     = Edge(new (2) Node(op, arg0, arg1));
      _table[index].hashVal = hashVal;
      ++_size;
    }

    return _table[index].key;
  }

  Edge GlobalTable::uNode(NodeOp op, Edge arg0, Edge arg1, Edge arg2) {
    if (_size == _maxSize) grow(_size + 1);
    assert(_size < _maxSize);
    
    uint32_t hashVal = hashInt32(op);
    hashCombine(hashVal, hashInt32(3));
    hashCombine(hashVal, hashPtr(arg0));
    hashCombine(hashVal, hashPtr(arg1));
    hashCombine(hashVal, hashPtr(arg2));

    size_t index = hashVal % _cap;
    if (!lookup(hashVal, op, arg0, arg1, arg2, index)) {
      _table[index].key     = Edge(new (3) Node(op, arg0, arg1, arg2));
      _table[index].hashVal = hashVal;
      ++_size;
    }

    return _table[index].key;
  }

  void GlobalTable::remove(Edge e) {
    uint32_t hashVal = hashInt32(e->op());
    hashCombine(hashVal, hashInt32(e->size()));
    for (size_t i = 0; i < e->size(); i++) hashCombine(hashVal, hashPtr((*e)[i]));

    size_t index = hashVal % _cap;
    if (lookup(index, e, index)) {
      _table[index].key = Edge((void*) NULL);
      --_size;
    }
  }
}  
