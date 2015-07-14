#include "ADTs/LitVector.H"

#include <cstring>
#include <cmath>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef MERGING
#error "Expected MERGING to be defined when compiling __FILE__."
#endif

#define MIN_LITVEC_SIZE 2

namespace nicesat {
  LitVector::LitVector()
    : _size(0), _alloc(MIN_LITVEC_SIZE),
      _lits((Literal*) malloc(MIN_LITVEC_SIZE * sizeof(Literal))) {
    // Intentionally Empty
  }

  LitVector::LitVector(const LitVector& other) : _size(other._size), _alloc(other._alloc),
                                                 _lits ((Literal*) malloc(_alloc * sizeof(Literal))) {
    memcpy(_lits, other._lits, other._size * sizeof(Literal));
  }

  void LitVector::init(const LitVector& other) {
    _size = other._size;
    if (other._alloc > _alloc) {
      _alloc = other._alloc;
      _lits = ((Literal*) realloc(_lits, _alloc * sizeof(Literal)));
    }

    memcpy(_lits, other._lits, other._size * sizeof(Literal));
  }

  void LitVector::init(Literal l) {
    _size = 1;
    _lits[0] = l;
  }

  void LitVector::init(Literal l1, Literal l2) {
    _size = 2;
#if MERGING > 1
    int l1abs = abs(l1);
    int l2abs = abs(l2);

    assert(l1abs != l2abs);
    if (l1abs < l2abs) {
      _lits[0] = l1;
      _lits[1] = l2;
    } else {
      _lits[0] = l2;
      _lits[1] = l1;
    }
#else
    _lits[0] = l1;
    _lits[1] = l2;
#endif
  }


  void LitVector::init(const LitVector& other, Literal l) {
    _size = other._size;

    int newAlloc = other._alloc;
    if (_size == newAlloc) newAlloc <<= 1;
    if (newAlloc > _alloc) {
      _alloc = newAlloc;
      _lits = (Literal*) realloc(_lits, _alloc * sizeof(Literal));
    }
    memcpy(_lits, other._lits, sizeof(Literal) * _size);
#if MERGING > 0
    addLit(l);
#else
    _lits[_size++] = l;
#endif
  }

  void LitVector::init(const LitVector& other1, const LitVector& other2) {
    _size = other1._size;

    int newAlloc = other1._alloc;
    if (newAlloc < other2._alloc) newAlloc = other2._alloc;
    if (newAlloc < other1._size + other2._size) newAlloc <<= 1;
    if (newAlloc > _alloc) {
      _alloc = newAlloc;
      _lits = (Literal*) realloc(_lits, sizeof(Literal) * _alloc);
    }
    
#if MERGING > 0
    if (_size >= MERGING) {
      memcpy(_lits, other1._lits, sizeof(Literal) * _size);
      addVector_(other2._lits, other2._size);
    } else if (other2._size >= MERGING) {
      memcpy(_lits, other2._lits, sizeof(Literal) * other2._size);
      _size = other2._size;
      addVector_(other1._lits, other1._size);
    } else {
      memcpy(_lits + other2._size, other1._lits, sizeof(Literal) * _size);
      addVector(other2);
    }
#else
    memcpy(_lits, other1._lits, sizeof(Literal) * _size);
    memcpy(_lits + other1._size, other2._lits, sizeof(Literal) * other2._size);
    _size += other2._size;
#endif
  }

  const LitVector& LitVector::operator+=(Literal l) {
    if (_size == _alloc) {
      _alloc <<= 1;
      _lits = (Literal*) realloc(_lits, sizeof(Literal) * _alloc);
    }

#if MERGING > 0
    addLit(l);
#else
    _lits[_size++] = l;
#endif
    return *this;
  }
  
  void LitVector::addLit(Literal l) {
    int lAbs = abs(l);
    int ub = MERGING; if (ub > _size) ub = _size;

    // Linear search for l (this could be converted to a binary search)
    int i = 0;
    for (; i < ub; i++) {
      int iAbs = abs(_lits[i]);
      if (iAbs > lAbs) break;
      if (iAbs == lAbs) {
        if (_lits[i] == -l) _size = 0;
        return;
      }
    }

    if (_size < MERGING) {
      memmove(_lits + i + 1, _lits + i, sizeof(Literal) * (_size - i));
      _lits[i] = l;
      _size++;
    } else {
      _lits[_size++] = l;
    }
  }

  void LitVector::addVector_(Literal* lits, int size) {
    assert(_size >= MERGING);
    
    int i = 0;
    int j = 0;
    int iUb = MERGING; if (iUb > _size) iUb = _size;
    int jUb = MERGING; if (jUb >  size) jUb =  size;

    if (i < iUb && j < jUb) {
      int iAbs = abs(_lits[i]);
      int jAbs = abs( lits[j]);

      while (true) {
        if (iAbs < jAbs) {
          if (++i == iUb) break;
          iAbs = abs(_lits[i]);
        } else if (jAbs < iAbs) {
          _lits[_size++] = lits[j];
          if (++j == jUb) break;
          jAbs = abs( lits[j]);
        } else if (_lits[i] == lits[j]) {
          ++i;
          if (++j == jUb || i == iUb) break;
          iAbs = abs(_lits[i]);
          jAbs = abs( lits[j]);
        } else {
          _size = 0; return;
        }
      }
    }

    memmove(_lits + _size, lits + j, sizeof(Literal) * (size - j));
    _size += (size - j);
  }

  void LitVector::addVector(const LitVector& other) {
    int lastI = _size + other._size;
    int i = other._size;
    int j = 0;

    int iUb = MERGING; if (iUb > _size) iUb = _size; iUb += other._size;
    int jUb = MERGING; if (jUb > other._size) jUb = other._size;

    _size = 0; // _size is used as the out pointer
    if (i < iUb && j < jUb) {
      int iAbs = abs(_lits[i]);
      int jAbs = abs(other._lits[j]);

      while (true) {
        if (iAbs < jAbs) {
          _lits[_size++] = _lits[i];
          if (++i == iUb) break;
          iAbs = abs(_lits[i]);
        } else if (jAbs < iAbs) {
          _lits[_size++] = other._lits[j];
          if (++j == jUb) break;
          jAbs = abs(other._lits[j]);
        } else if (_lits[i] == other._lits[j]) {
          _lits[_size++] = other._lits[j];
          ++i;
          if (++j == jUb || i == iUb) break;
          iAbs = abs(_lits[i]);
          jAbs = abs(other._lits[j]);
        } else {
          _size = 0; return;
        }
      }
    }

    int otherRem = other._size - j;
    assert(otherRem >= 0);
    memcpy(_lits + _size, other._lits + j, sizeof(Literal) * otherRem);
    _size += otherRem;

    int selfRem = lastI - i;
    assert(selfRem >= 0);
    memmove(_lits + _size, _lits + i, sizeof(Literal) * selfRem);
    _size += selfRem;
  }
  
  const LitVector& LitVector::operator+=(const LitVector& other) {
    int newSize = _size + other._size;

    int oldAlloc = _alloc;
    if (other._alloc > _alloc) _alloc = other._alloc;
    if (newSize > _alloc) _alloc <<= 1;
    assert(_alloc >= newSize);
    if (_alloc != oldAlloc) _lits = (Literal*) realloc(_lits, sizeof(Literal) * _alloc);
    
#if MERGING > 0
    if (_size >= MERGING) {
      addVector_(other._lits, other._size);
    } else {
      memmove(_lits + other._size, _lits, sizeof(Literal) * _size);
      addVector(other);
    }
#else
    memcpy(_lits + _size, other._lits, sizeof(Literal) * other._size);
    _size += other._size;
#endif
    return *this;
  }

  const LitVector& LitVector::operator=(const LitVector& other) {
    _size = other._size;
    if (other._alloc > _alloc) {
      _alloc = other._alloc;
      _lits = (Literal*) realloc(_lits, sizeof(Literal) * _alloc);
    }
    memcpy(_lits, other._lits, sizeof(Literal) * other._size);
    return *this;
  }

  void LitVector::swap(LitVector& other) {
    Literal* tmpLits = _lits;
    int tmpSize      = _size;
    int tmpAlloc     = _alloc;
    _size  = other._size;
    _alloc = other._alloc;
    _lits  = other._lits;
    other._size  = tmpSize;
    other._alloc = tmpAlloc;
    other._lits  = tmpLits;
  }
}

