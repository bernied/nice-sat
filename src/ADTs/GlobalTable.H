#ifndef NICESAT_ADTS_GLOBAL_TABLE_H_
#define NICESAT_ADTS_GLOBAL_TABLE_H_

#include "ADTs/Edge.H"
#include "ADTs/Node.H"
#include "ADTs/HashTable.H"
#include <stdlib.h>

namespace nicesat {
  class GlobalTable {
  private:
    struct HashEntry { uint32_t hashVal; Edge key; };
    HashEntry* _table;
    size_t _size;
    size_t _maxSize;
    size_t _cap;

    bool lookup(uint32_t hashVal, NodeOp op, const Vec<Edge>& args, size_t& index);
    bool lookup(uint32_t hashVal, NodeOp op, Edge arg0, Edge arg1, size_t& index);
    bool lookup(uint32_t hashVal, NodeOp op, Edge arg0, Edge arg1, Edge arg2, size_t& index);
    bool lookup(uint32_t hashVal, Edge e, size_t& index);

  public:
    GlobalTable() : _table(NULL), _size(0), _maxSize(0), _cap(0) { grow(1); }
    ~GlobalTable() { clear(true); }

    void grow(size_t size);
    void clear(bool freeMem = false);

    // Functions for interacting with the global table
    Edge uNode(NodeOp op, const Vec<Edge>& args);
    Edge uNode(NodeOp op, Edge arg0, Edge arg1);
    Edge uNode(NodeOp op, Edge arg0, Edge arg1, Edge arg2);

    // Remove the edge from the control of the global table
    void remove(Edge e);

  };
}

#endif//NICESAT_ADTS_GLOBAL_TABLE_H_
