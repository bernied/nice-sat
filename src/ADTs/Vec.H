#ifndef NICESAT_ADTS_VEC_H_
#define NICESAT_ADTS_VEC_H_

#include <cstring>
#include <cassert>
#include "Support/Shared.H"

#define MIN_VECTOR_CAP 2

namespace nicesat {
  template<class T>
  class Vec {
    size_t _size;
    size_t _cap; 
    T*     _data;
    T*     _alloc;
    
  public:
    Vec() : _size(0), _cap(0), _data(NULL), _alloc(NULL) {};
    Vec(size_t initCap) : _size(0), _cap(0), _data(NULL), _alloc(NULL) { grow(initCap); }
    Vec(const Vec<T>& other) : _size(other.size), _cap(other._cap), _data(NULL), _alloc(NULL) {
      grow(_cap);
      memcpy(_data, other._data, _size * sizeof(T));
    }
    ~Vec() { free(_alloc); }

    // Memory operations
    void grow(size_t newCap);
    void clear(bool freeData = false);

    // Stack Operations
    void push ()              { if (_size == _cap) grow(_cap + 1); _size++; }
    void push (const T& elem) { if (_size == _cap) grow(_cap + 1); _data[_size++] = elem; }
    void push_(const T& elem) { assert(_size < _cap); _data[_size++] = elem; }

    void shrinkBack(size_t nElem)  { _size -= nElem; }
    void shrinkFront(size_t nElem) { _size -= nElem; _cap -= nElem; _data += nElem; }

    void pop()      { --_size; }
    void popFront() { --_size; --_cap; ++_data; } // wastes space until compact called

    // Accessors
    size_t size() const { return _size; }
    
    const T& first() const { return _data[0]; }
    T&       first()       { return _data[0]; }
    const T& last()  const { return _data[_size - 1]; }
    T&       last()        { return _data[_size - 1]; }
    
    const T& operator[] (int idx) const { return _data[idx]; }
    T&       operator[] (int idx)       { return _data[idx]; }

    T*       c_array() const { return _data; }
  };

  template<class T>
  void Vec<T>::grow(size_t newCap) {
     if (_cap == 0) _cap = MIN_VECTOR_CAP;
     _cap += (size_t) (_data - _alloc);
     while (_cap < newCap) _cap <<= 1;

     // If we have wasted space in the front, do something smart
     if (_data != _alloc) {
        T* newAlloc = (T*) malloc(sizeof(T) * _cap);
        memcpy(newAlloc, _data, sizeof(T) * _size);
        free(_alloc);
        _data = _alloc = newAlloc;
     } else {
        _data = _alloc = (T*) realloc(_alloc, sizeof(T) * _cap);
     }
  }

  template<class T>
  void Vec<T>::clear(bool freeData) {
    _size = 0;
    if (freeData) {
      free(_alloc);
      _alloc = _data = NULL;
    } else {
      // if not freeing data, reclaim any wasted space at the front
      _cap += (_alloc - _data);
      _data = _alloc;
    }
  }
}; // namespace nicesat

#endif//NICESAT_ADTS_VEC_H_
