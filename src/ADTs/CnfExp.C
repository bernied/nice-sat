#include "ADTs/CnfExp.H"

#include <ostream>
#include <cassert>
#include <cstring>
using namespace std;

#include "Support/Shared.H"

namespace nicesat {
  CnfExp::Clause* CnfExp::_pool = NULL;
  
  CnfExp::CnfExp(bool isTrue)
    : _litSize(0), _clauseSize(isTrue ? 1 : 0), _singletons(), _head(NULL), _tail(NULL) { }
  
  CnfExp::CnfExp(Literal lit)
    : _litSize(1), _clauseSize(1), _singletons(), _head(NULL), _tail(NULL) { _singletons.init(lit); }

  void CnfExp::clear(bool isTrue) {
    _singletons.clear();
    if (_tail) {
      _tail->_next = _pool;
      _pool = _head;
      
      _head = NULL;
      _tail = NULL;
    }

    _litSize    = 0;
    _clauseSize = (isTrue ? 1 : 0);
  }
  
  void CnfExp::conjoin(Literal lit) {
    if (_litSize == 0) {
      if (_clauseSize == 0) return;
      _clauseSize = 0;
    }

    _litSize -= _singletons.size();
    _clauseSize -= _singletons.size();
    _singletons += lit;
    if (_singletons.size() == 0) {
      clear(false); // contradiction detected
    } else {
      _litSize += _singletons.size();
      _clauseSize += _singletons.size();
    }
  }

  void CnfExp::print(ostream& os) {
    for (int i = 0; i < _singletons.size(); i++) {
      os << "(" << _singletons[i] << ")\n";
    }

    Clause* iter = _head;
    while (iter) {
      os << "(" << iter->_litVec[0];
      for (int i = 1; i < iter->_litVec.size(); i++) {
        os << " " << iter->_litVec[i];
      }
      os << ")\n";
      iter = iter->_next;
    }

    os << endl;
  }

  void CnfExp::conjoin(CnfExp* exp, bool destroy) {
    if (exp->_litSize == 0) {
      if (exp->_clauseSize == 0) clear(false);
      if (destroy) delete exp;
      return;
    }

    if (_litSize == 0) {
      if (_clauseSize != 0) {
        copyExp(exp, destroy);
      } else if (destroy) {
        delete exp;
      }
      return;
    }
    
    // conjoin the singletons
    bool hadSingletons = false;
    if (exp->_singletons.size() != 0) {
      if (_singletons.size() != 0) {
        hadSingletons = true;
        _litSize -= _singletons.size();
        _clauseSize -= _singletons.size();
      }
      _singletons += exp->_singletons;
      _litSize    += _singletons.size();
      _clauseSize += _singletons.size();
    }
    
    if (hadSingletons && _singletons.size() == 0) { // contradiction detected
      clear(false);
      if (destroy) delete exp;
    } else if (destroy) {
      if (exp->_head != NULL) {
        if (_head == NULL) {
          _head = exp->_head;
        } else {
          _tail->_next = exp->_head;
        }
        _tail = exp->_tail;
        _clauseSize += exp->_clauseSize - exp->_singletons.size();
        _litSize += exp->_litSize - exp->_singletons.size();

        exp->_head = NULL;
        exp->_tail = NULL;
      }
      
      delete exp;
    } else {
      Clause* iter = exp->_head;
      if (iter != NULL && _head == NULL) {
        _tail = _head = newClause();
        _head->_litVec.init(iter->_litVec);
        iter = iter->_next;
      }
      
      while (iter) {
        _head = newClause(_head);
        _head->_litVec.init(iter->_litVec);
        _litSize += _head->_litVec.size();
        ++_clauseSize;
        iter = iter->_next;
      }
    }
  }

  void CnfExp::disjoin(Literal lit) {
    // Handle disjoining with false
    if (_litSize == 0) {
      if (_clauseSize == 0) {
        _singletons += lit;
        _litSize = 1;
        _clauseSize = 1;
      }
      return;
    }
    
    Clause *prev = NULL, *iter = _head;
    // Find the first non-merged clause
    while (iter != NULL) {
      _litSize -= iter->_litVec.size();
      iter->_litVec += lit;
      if (iter->_litVec.size() == 0) {
        --_clauseSize;
        _head = _head->_next;
        delete iter;
        iter = _head;
      } else {
        _litSize += iter->_litVec.size();
        prev = iter;
        iter = iter->_next;
        break;
      }
    }

    // Handle disjoining with the rest of the clauses
    while (iter != NULL) {
      _litSize -= iter->_litVec.size();
      iter->_litVec += lit;
      if (iter->_litVec.size() == 0) {
        prev->_next = iter->_next;
        delete iter;
        iter = prev->_next;
        --_clauseSize;
      } else {
        _litSize += iter->_litVec.size();
        prev = iter;
        iter = iter->_next;
      }
    }

    _tail = prev;

    // Disjoin with the singletons
    int out = 0;
    for (int i = 0; i < _singletons.size(); i++) {
      if (_singletons[i] == lit) {
        _singletons[out++] = lit;
      } else if (_singletons[i] != -lit) {
        ++_litSize;
        _head = newClause(_head);
        _head->_litVec.init(_singletons[i], lit);
        if (_tail == NULL) _tail = _head;
      } else {
        --_litSize;
        --_clauseSize;
      }
    }
    _singletons.setSize(out);

    if (_clauseSize == 0) {
      _litSize = 0;
      _clauseSize = 1;
    }
  }

  void CnfExp::copyExp(CnfExp* exp, bool destroy) {
    assert(_head == NULL);
    assert(_tail == NULL);

    _clauseSize = exp->_clauseSize;
    _litSize    = exp->_litSize;
    if (destroy) {
      _head = exp->_head;
      _tail = exp->_tail;
      _singletons.swap(exp->_singletons);
      exp->_head = NULL;
      exp->_tail = NULL;
      delete exp;
    } else { // if not, we need to make an actual copy
      _singletons = exp->_singletons;

      Clause* iter2 = exp->_head;
      if (iter2) {
        _tail = _head = newClause(_head);
        _head->_litVec.init(iter2->_litVec);
        iter2 = iter2->_next;
      }

      while (iter2) {
        _head = newClause(_head);
        _head->_litVec.init(iter2->_litVec);
        iter2 = iter2->_next;
      }
    }
  }

  void CnfExp::disjoin(CnfExp* exp, bool destroy) {
    if (exp->_litSize == 0) {       // when disjoining with a constant
      if (exp->_clauseSize > 0) clear(true);
      return;
    }
    
    if (_litSize == 0) {  // if this is const
      assert(_head == _tail && _tail == NULL);
      assert(_singletons.size() == 0);

      if (_clauseSize == 0) {   // if this is false, we need to copy exp
        copyExp(exp, destroy);
      } else if (destroy) {     // if this is true, then we're still true
        delete exp;
      }
    } else { // otherwise, we need to compute the cross product
      crossProductWith(exp);
      if (destroy) delete exp;
    }
  }

  void CnfExp::crossProductWith(CnfExp* exp) {
    Clause* oldHead = _head; Clause* oldTail = _tail; _head = NULL; _tail = NULL;

    // Important: A clause union a singleton will always produce
    // either an empty clause, or a clause of size > 2.  Thus we don't
    // have to check to see if we should make a new singleton unless
    // it is singleton union singleton (and the two are equal)

    _clauseSize = 0;
    _litSize = 0;

    // 1. Cross-product clauses with clauses
    // 
    // if there are no singletons on the other side, we want to
    // destructively use the already allocated clauses for the last
    // clause in exp -- thus we leave it for later.  If there are
    // singletons, we can run to the end
    Clause* iter2 = exp->_head;
    Clause* iter2end = (exp->_singletons.size() > 0 ? NULL : exp->_tail);
    while (iter2 != iter2end) {
      Clause* iter1 = oldHead;
      const LitVector& litVec2 = iter2->_litVec;
      while (iter1) {
        _head = newClause(_head);
        _head->_litVec.init(iter1->_litVec, litVec2);
        if (_head->_litVec.size() == 0) {
          Clause* old = _head;
          _head = _head->_next;
          delete old;
        } else {
          if (_tail == NULL) _tail = _head;
          ++_clauseSize;
          _litSize += _head->_litVec.size();
        }
        iter1 = iter1->_next;
      }
      iter2 = iter2->_next;
    }

    if (exp->_singletons.size() > 0) {
      // 2. Cross-product clauses with singletons
      for (int i = 1; i < exp->_singletons.size(); i++) {
        Literal lit = exp->_singletons[i];
        Clause* iter1 = oldHead;
        while (iter1) {
          _head = newClause(_head);
          _head->_litVec.init(iter1->_litVec, lit);
          if (_head->_litVec.size() == 0) {
            Clause* old = _head;
            _head = _head->_next;
            delete old;
          } else {
            if (_tail == NULL) _tail = _head;
            ++_clauseSize;
            _litSize += _head->_litVec.size();
          }
          iter1 = iter1->_next;
        }
      }

      // Last singleton is destructively crossed with the old clauses
      Clause* prev = NULL;
      Clause* iter = oldHead;
      Literal lit = exp->_singletons[0];

      while (iter) {
        iter->_litVec += lit;
        if (iter->_litVec.size() == 0) {
          if (prev == NULL) {
            oldHead = iter->_next;
            delete iter;
            iter = oldHead;
          } else {
            prev->_next = iter->_next;
            delete iter;
            iter = prev->_next;
          }
        } else {
          ++_clauseSize;
          _litSize += iter->_litVec.size();
          prev = iter;
          iter = iter->_next;
        }
      }
      oldTail = prev;
    } else if (exp->_tail) {
      // No singletons -- destructively update the last clause
      Clause* prev = NULL;
      Clause* iter = oldHead;
      const LitVector& litVec2 = exp->_tail->_litVec;
      
      while (iter) {
        iter->_litVec += litVec2;
        if (iter->_litVec.size() == 0) {
          if (prev == NULL) {
            oldHead = iter->_next;
            delete iter;
            iter = oldHead;
          } else {
            prev->_next = iter->_next;
            delete iter;
            iter = prev->_next;
          }
        } else {
          ++_clauseSize;
          _litSize += iter->_litVec.size();
          prev = iter;
          iter = iter->_next;
        }
      }
      oldTail = prev;
    } else {
      Clause* iter = oldHead;
      while (iter) {
        oldHead = iter->_next;
        delete iter;
        iter = oldHead;
      }
      oldTail = NULL;
    }

    if (oldTail != NULL) {
      if (_head == NULL) {
        assert(_tail == NULL);
        _tail = oldTail;
      } else {
        oldTail->_next = _head;
      }
      _head = oldHead;
    }

    // 3. Cross product singletons and exp->clauses
    if (exp->_head) { // don't bother with outer loop if no inner loop
      for (int i = 0; i < _singletons.size(); i++) {
        Clause* iter2 = exp->_head;
        while (iter2) {
          _head = newClause(_head);
          _head->_litVec.init(iter2->_litVec, _singletons[i]);
          if (_head->_litVec.size() == 0) {
            Clause* old = _head;
            _head = _head->_next;
            delete old;
          } else {
            if (_tail == NULL) _tail = _head;
            ++_clauseSize;
            _litSize += _head->_litVec.size();
          }
          iter2 = iter2->_next;
        }
      }
    }

    // 4. Cross product singletons and exp->singletons.
    int out = 0;
    if (exp->_singletons.size() > 0) { // don't bother with the outer loop if no inner loop
      for (int i = 0; i < _singletons.size(); i++) {
        for (int j = 0; j < exp->_singletons.size(); j++) {
          if (_singletons[i] == exp->_singletons[j]) {
            ++_clauseSize;
            ++_litSize;
            _singletons[out++] = exp->_singletons[j];
          } else if (_singletons[i] != -exp->_singletons[j]) {
            _head = newClause(_head);
            _head->_litVec.init(_singletons[i], exp->_singletons[j]);
            if (_tail == NULL) _tail = _head;
            ++_clauseSize;
            _litSize += 2;
          }
        }
      }
    }
    _singletons.setSize(out);

    // 5. If all clauses disappear, make the clause true (not false)
    if (_head == NULL && _singletons.size() == 0) {
      assert(_litSize == 0);
      _clauseSize = 1;
    }
  }
};
