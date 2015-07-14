#include <cppunit/extensions/TestFactoryRegistry.h>
#include "ADTs/CnfExpTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(CnfExpTests);

#include "ADTs/CnfExp.H"
using namespace nicesat;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef MERGING
#error "Expected MERGING to be defined when compiling __FILE__."
#endif

void CnfExpTests::setUp() {
  a = new CnfExp(1);
  b = new CnfExp(2);
}

void CnfExpTests::tearDown() {
  delete a;
  delete b;
}

bool eqLitVec(const LitVector& litVec, Literal* lits, int litSize) {
  if (litVec.size() != litSize) return false;

  bool* seen = new bool[litSize];
  for (int i = 0; i < litSize; i++) seen[i] = false;

  bool ans = true;
  for (int i = 0; i < litSize; i++) {
    bool found = false;
    for (int j = 0; j < litSize; j++) {
      if (!seen[j] && lits[j] == litVec[i]) {
        seen[j] = true;
        found   = true;
        break;
      }
    }
    
    if (!found) {
      ans = false;
      break;
    }
  }
  
  if (ans) {
    for (int i = 0; i < litSize; i++) if (!seen[i]) { ans = false; break; }
  }
  delete[] seen;
  return ans;
}

bool CnfExpTests::containsClause(CnfExp* exp, Literal* lits, int litSize) {
  if (litSize == 1) {
    const LitVector& singletons = exp->singletons();
    for (int i = 0; i < singletons.size(); i++) {
      if (singletons[i] == lits[0]) return true;
    }
  } else {
    CnfExp::Clause* i = exp->head();
    while (i) {
      if (i->_litVec.size() == litSize) {
        if (eqLitVec(i->_litVec, lits, litSize)) return true;
      }
      i = i->_next;
    }
  }
  return false;
}

#define ASSERT_CONTAINS(expPtr, ...)                                    \
  do {                                                                  \
    Literal arr[] = { __VA_ARGS__ };                                    \
    CPPUNIT_ASSERT(containsClause(expPtr, arr, sizeof(arr) / sizeof(arr[0]))); \
  } while (0)
#define ASSERT_NOT_CONTAINS(expPtr, ...)                                \
  do {                                                                  \
    Literal arr[] = { __VA_ARGS__ };                                    \
    CPPUNIT_ASSERT(!containsClause(expPtr, arr, sizeof(arr) / sizeof(arr[0]))); \
  } while (0)


void CnfExpTests::construction() {
  CPPUNIT_ASSERT_EQUAL(1, a->litSize());
  CPPUNIT_ASSERT_EQUAL(1, a->clauseSize());
  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  ASSERT_CONTAINS(a, 1);

  CPPUNIT_ASSERT_EQUAL(1, b->litSize());
  CPPUNIT_ASSERT_EQUAL(1, b->clauseSize());
  CPPUNIT_ASSERT_EQUAL(1, b->singletons().size());
  ASSERT_CONTAINS(b, 2);
}

void CnfExpTests::conjoinLit_singles() {
  a->conjoin(2);
  ASSERT_CONTAINS(a, 1);
  ASSERT_CONTAINS(a, 2);
  CPPUNIT_ASSERT_EQUAL(2, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(2, a->litSize());
  CPPUNIT_ASSERT_EQUAL(2, a->clauseSize());
}

void CnfExpTests::conjoinExp_destroy_singles() {
  a->conjoin(b, true);
  ASSERT_CONTAINS(a, 1);
  ASSERT_CONTAINS(a, 2);
  CPPUNIT_ASSERT_EQUAL(2, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(2, a->litSize());
  CPPUNIT_ASSERT_EQUAL(2, a->clauseSize());
  
  b = NULL; // now the only way b's memory will be freed is if conjoin
            // takes care of that
}

void CnfExpTests::conjoinExp_nodestroy_singles() {
  a->conjoin(b, false);
  ASSERT_CONTAINS(a, 1);
  ASSERT_CONTAINS(a, 2);
  CPPUNIT_ASSERT_EQUAL(2, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(2, a->litSize());
  CPPUNIT_ASSERT_EQUAL(2, a->clauseSize());
  
  ASSERT_CONTAINS(b, 2);
  CPPUNIT_ASSERT_EQUAL(1, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(1, b->litSize());
  CPPUNIT_ASSERT_EQUAL(1, b->clauseSize());
}

void CnfExpTests::disjoinLit_singles() {
  a->disjoin(2);
  ASSERT_CONTAINS(a, 1, 2);
  CPPUNIT_ASSERT_EQUAL(0, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(2, a->litSize());
  CPPUNIT_ASSERT_EQUAL(1, a->clauseSize());
}

void CnfExpTests::prepareExps() {
  a->conjoin(3);
  a->disjoin(4);
  a->conjoin(5); // a = (1 4) (3 4) (5)

  b->conjoin(6);
  b->conjoin(7); 
  b->disjoin(8); // b = (2 8) (6 8) (7 8)
}

void CnfExpTests::check_prepareExps() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)
  
  ASSERT_CONTAINS(a, 1, 4);
  ASSERT_CONTAINS(a, 3, 4);
  ASSERT_CONTAINS(a, 5);
  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(5, a->litSize());
  CPPUNIT_ASSERT_EQUAL(3, a->clauseSize());

  ASSERT_CONTAINS(b, 2, 8);
  ASSERT_CONTAINS(b, 6, 8);
  ASSERT_CONTAINS(b, 7, 8);
  CPPUNIT_ASSERT_EQUAL(0, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(6, b->litSize());
  CPPUNIT_ASSERT_EQUAL(3, b->clauseSize());
}

void CnfExpTests::conjoinExp_destroy_a() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)
  
  a->conjoin(b, true);
  ASSERT_CONTAINS(a, 1, 4);
  ASSERT_CONTAINS(a, 3, 4);
  ASSERT_CONTAINS(a, 5);
  ASSERT_CONTAINS(a, 2, 8);
  ASSERT_CONTAINS(a, 6, 8);
  ASSERT_CONTAINS(a, 7, 8);

  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(11, a->litSize());
  CPPUNIT_ASSERT_EQUAL(6, a->clauseSize());
  
  b = NULL; // now the only way b's memory will be freed is if conjoin
            // takes care of that
}

void CnfExpTests::conjoinExp_nodestroy_a() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)

  a->conjoin(b, false);
  ASSERT_CONTAINS(a, 1, 4);
  ASSERT_CONTAINS(a, 3, 4);
  ASSERT_CONTAINS(a, 5);
  ASSERT_CONTAINS(a, 2, 8);
  ASSERT_CONTAINS(a, 6, 8);
  ASSERT_CONTAINS(a, 7, 8);

  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(11, a->litSize());
  CPPUNIT_ASSERT_EQUAL(6, a->clauseSize());

  ASSERT_CONTAINS(b, 2, 8);
  ASSERT_CONTAINS(b, 6, 8);
  ASSERT_CONTAINS(b, 7, 8);
  CPPUNIT_ASSERT_EQUAL(0, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(6, b->litSize());
  CPPUNIT_ASSERT_EQUAL(3, b->clauseSize());
}


void CnfExpTests::conjoinExp_destroy_b() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)
  
  b->conjoin(a, true);
  ASSERT_CONTAINS(b, 1, 4);
  ASSERT_CONTAINS(b, 3, 4);
  ASSERT_CONTAINS(b, 5);
  ASSERT_CONTAINS(b, 2, 8);
  ASSERT_CONTAINS(b, 6, 8);
  ASSERT_CONTAINS(b, 7, 8);

  CPPUNIT_ASSERT_EQUAL(1, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(11, b->litSize());
  CPPUNIT_ASSERT_EQUAL(6, b->clauseSize());
  
  a = NULL; // now the only way b's memory will be freed is if conjoin
            // takes care of that
}

void CnfExpTests::conjoinExp_nodestroy_b() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)

  b->conjoin(a, false);
  ASSERT_CONTAINS(b, 1, 4);
  ASSERT_CONTAINS(b, 3, 4);
  ASSERT_CONTAINS(b, 5);
  ASSERT_CONTAINS(b, 2, 8);
  ASSERT_CONTAINS(b, 6, 8);
  ASSERT_CONTAINS(b, 7, 8);


  CPPUNIT_ASSERT_EQUAL(1, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(11, b->litSize());
  CPPUNIT_ASSERT_EQUAL(6, b->clauseSize());

  ASSERT_CONTAINS(a, 1, 4);
  ASSERT_CONTAINS(a, 3, 4);
  ASSERT_CONTAINS(a, 5);
  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(5, a->litSize());
  CPPUNIT_ASSERT_EQUAL(3, a->clauseSize());
}

void CnfExpTests::disjoinExp_destroy_a() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)
  a->disjoin(b, true); b = NULL;

  ASSERT_CONTAINS(a, 1, 4, 2, 8);
  ASSERT_CONTAINS(a, 1, 4, 6, 8);
  ASSERT_CONTAINS(a, 1, 4, 7, 8);
  ASSERT_CONTAINS(a, 3, 4, 2, 8);
  ASSERT_CONTAINS(a, 3, 4, 6, 8);
  ASSERT_CONTAINS(a, 3, 4, 7, 8);
  ASSERT_CONTAINS(a, 5, 2, 8);
  ASSERT_CONTAINS(a, 5, 6, 8);
  ASSERT_CONTAINS(a, 5, 7, 8);

  CPPUNIT_ASSERT_EQUAL(0, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(9, a->clauseSize());
  CPPUNIT_ASSERT_EQUAL(33, a->litSize());
}

void CnfExpTests::disjoinExp_nodestroy_a() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)
  a->disjoin(b, false);

  ASSERT_CONTAINS(a, 1, 4, 2, 8);
  ASSERT_CONTAINS(a, 1, 4, 6, 8);
  ASSERT_CONTAINS(a, 1, 4, 7, 8);
  ASSERT_CONTAINS(a, 3, 4, 2, 8);
  ASSERT_CONTAINS(a, 3, 4, 6, 8);
  ASSERT_CONTAINS(a, 3, 4, 7, 8);
  ASSERT_CONTAINS(a, 5, 2, 8);
  ASSERT_CONTAINS(a, 5, 6, 8);
  ASSERT_CONTAINS(a, 5, 7, 8);

  CPPUNIT_ASSERT_EQUAL(0, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(9, a->clauseSize());
  CPPUNIT_ASSERT_EQUAL(33, a->litSize());

  ASSERT_CONTAINS(b, 2, 8);
  ASSERT_CONTAINS(b, 6, 8);
  ASSERT_CONTAINS(b, 7, 8);
  CPPUNIT_ASSERT_EQUAL(0, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(6, b->litSize());
  CPPUNIT_ASSERT_EQUAL(3, b->clauseSize());
}

void CnfExpTests::disjoinExp_destroy_b() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)
  b->disjoin(a, true); a = NULL;

  ASSERT_CONTAINS(b, 1, 4, 2, 8);
  ASSERT_CONTAINS(b, 1, 4, 6, 8);
  ASSERT_CONTAINS(b, 1, 4, 7, 8);
  ASSERT_CONTAINS(b, 3, 4, 2, 8);
  ASSERT_CONTAINS(b, 3, 4, 6, 8);
  ASSERT_CONTAINS(b, 3, 4, 7, 8);
  ASSERT_CONTAINS(b, 5, 2, 8);
  ASSERT_CONTAINS(b, 5, 6, 8);
  ASSERT_CONTAINS(b, 5, 7, 8);

  CPPUNIT_ASSERT_EQUAL(0, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(9, b->clauseSize());
  CPPUNIT_ASSERT_EQUAL(33, b->litSize());
}

void CnfExpTests::disjoinExp_nodestroy_b() {
  prepareExps(); // a = (1 4) (3 4) (5), b = (2 8) (6 8) (7 8)
  b->disjoin(a, false);

  ASSERT_CONTAINS(b, 1, 4, 2, 8);
  ASSERT_CONTAINS(b, 1, 4, 6, 8);
  ASSERT_CONTAINS(b, 1, 4, 7, 8);
  ASSERT_CONTAINS(b, 3, 4, 2, 8);
  ASSERT_CONTAINS(b, 3, 4, 6, 8);
  ASSERT_CONTAINS(b, 3, 4, 7, 8);
  ASSERT_CONTAINS(b, 5, 2, 8);
  ASSERT_CONTAINS(b, 5, 6, 8);
  ASSERT_CONTAINS(b, 5, 7, 8);
  
  CPPUNIT_ASSERT_EQUAL(0, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(9, b->clauseSize());
  CPPUNIT_ASSERT_EQUAL(33, b->litSize());

  ASSERT_CONTAINS(a, 1, 4);
  ASSERT_CONTAINS(a, 3, 4);
  ASSERT_CONTAINS(a, 5);
  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(5, a->litSize());
  CPPUNIT_ASSERT_EQUAL(3, a->clauseSize());
}

void CnfExpTests::disjoinExp_nodestroy_singletons() {
  a->conjoin(3); 
  a->conjoin(4);  // a = (1) (3) (4)
    
  b->conjoin(4);
  b->conjoin(-3); // b = (2) (4) (-3)
  
  a->disjoin(b, false); // a = (1 2) (1 4) (1 -3) (3 2) (3 4) (4 2) (4) (4 -3)

  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(8, a->clauseSize());
  CPPUNIT_ASSERT_EQUAL(15, a->litSize());

  ASSERT_CONTAINS(a, 1, 2);
  ASSERT_CONTAINS(a, 1, 4);
  ASSERT_CONTAINS(a, 1, -3);
  ASSERT_CONTAINS(a, 3, 2);
  ASSERT_CONTAINS(a, 3, 4);
  ASSERT_CONTAINS(a, 4, 2);
  ASSERT_CONTAINS(a, 4);
  ASSERT_CONTAINS(a, 4, -3);

  CPPUNIT_ASSERT_EQUAL(3, b->singletons().size());
  CPPUNIT_ASSERT_EQUAL(3, b->clauseSize());
  CPPUNIT_ASSERT_EQUAL(3, b->litSize());
}

void CnfExpTests::disjoinExp_destroy_singletons() {
  a->conjoin(3); 
  a->conjoin(4);  // a = (1) (3) (4)
    
  b->conjoin(4);
  b->conjoin(-3); // b = (2) (4) (-3)
  
  a->disjoin(b, true); b = NULL; // a = (1 2) (1 4) (1 -3) (3 2) (3 4) (4 2) (4) (4 -3)

  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(8, a->clauseSize());
  CPPUNIT_ASSERT_EQUAL(15, a->litSize());

  ASSERT_CONTAINS(a, 1, 2);
  ASSERT_CONTAINS(a, 1, 4);
  ASSERT_CONTAINS(a, 1, -3);
  ASSERT_CONTAINS(a, 3, 2);
  ASSERT_CONTAINS(a, 3, 4);
  ASSERT_CONTAINS(a, 4, 2);
  ASSERT_CONTAINS(a, 4);
  ASSERT_CONTAINS(a, 4, -3);
}

void CnfExpTests::true_disjoin_lit() {
  CnfExp x(true);
  CPPUNIT_ASSERT(x.alwaysTrue());
  x.disjoin(1);
  CPPUNIT_ASSERT(x.alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(0, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, x.litSize());
  CPPUNIT_ASSERT_EQUAL(1, x.clauseSize());
}

void CnfExpTests::false_disjoin_lit() {
  CnfExp x(false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  x.disjoin(1);
  CPPUNIT_ASSERT(!x.alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(1, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(1, x.litSize());
  CPPUNIT_ASSERT_EQUAL(1, x.clauseSize());
  ASSERT_CONTAINS(&x, 1);
}

void CnfExpTests::true_disjoin_exp_nodestroy() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(true);
  CPPUNIT_ASSERT(x.alwaysTrue());
  x.disjoin(a, false);
  CPPUNIT_ASSERT(x.alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(0, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, x.litSize());
  CPPUNIT_ASSERT_EQUAL(1, x.clauseSize());

  ASSERT_CONTAINS(a, 1, 3);
  ASSERT_CONTAINS(a, 2, 3);
}

void CnfExpTests::true_disjoin_exp_destroy() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(true);
  CPPUNIT_ASSERT(x.alwaysTrue());
  x.disjoin(a, true); a = NULL;
  
  CPPUNIT_ASSERT(x.alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(0, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, x.litSize());
  CPPUNIT_ASSERT_EQUAL(1, x.clauseSize());
}

void CnfExpTests::exp_disjoin_true() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(true);
  CPPUNIT_ASSERT(x.alwaysTrue());
  a->disjoin(&x, false);
  CPPUNIT_ASSERT(x.alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(0, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, a->litSize());
  CPPUNIT_ASSERT_EQUAL(1, a->clauseSize());
}

void CnfExpTests::false_disjoin_exp_nodestroy() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  x.disjoin(a, false);
  CPPUNIT_ASSERT(!x.alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(0, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(4, x.litSize());
  CPPUNIT_ASSERT_EQUAL(2, x.clauseSize());
  ASSERT_CONTAINS(&x, 1, 3);
  ASSERT_CONTAINS(&x, 2, 3);

  ASSERT_CONTAINS(a, 1, 3);
  ASSERT_CONTAINS(a, 2, 3);
}

void CnfExpTests::false_disjoin_exp_destroy() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  x.disjoin(a, true); a = NULL;
  
  CPPUNIT_ASSERT(!x.alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(0, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(4, x.litSize());
  CPPUNIT_ASSERT_EQUAL(2, x.clauseSize());
  ASSERT_CONTAINS(&x, 1, 3);
  ASSERT_CONTAINS(&x, 2, 3);
}

void CnfExpTests::exp_disjoin_false() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  a->disjoin(&x, false);
  CPPUNIT_ASSERT(!a->alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(0, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(4, a->litSize());
  CPPUNIT_ASSERT_EQUAL(2, a->clauseSize());
  ASSERT_CONTAINS(a, 1, 3);
  ASSERT_CONTAINS(a, 2, 3);
}

void CnfExpTests::true_conjoin_lit() {
  CnfExp x(true);
  CPPUNIT_ASSERT(x.alwaysTrue());
  x.conjoin(1);
  CPPUNIT_ASSERT(!x.alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(1, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(1, x.litSize());
  CPPUNIT_ASSERT_EQUAL(1, x.clauseSize());
  ASSERT_CONTAINS(&x, 1);
}

void CnfExpTests::false_conjoin_lit() {
  CnfExp x(false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  x.conjoin(1);
  CPPUNIT_ASSERT(x.alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(0, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, x.litSize());
  CPPUNIT_ASSERT_EQUAL(0, x.clauseSize());

}

void CnfExpTests::true_conjoin_exp_nodestroy() {
  a->disjoin(2);
  a->conjoin(3); // a = (1 2) (3)
  
  CnfExp x(true);
  CPPUNIT_ASSERT(x.alwaysTrue());
  x.conjoin(a, false);
  CPPUNIT_ASSERT(!x.alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(1, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(3, x.litSize());
  CPPUNIT_ASSERT_EQUAL(2, x.clauseSize());
  ASSERT_CONTAINS(&x, 1, 2);
  ASSERT_CONTAINS(&x, 3);
    
  ASSERT_CONTAINS(a, 1, 2);
  ASSERT_CONTAINS(a, 3);
}

void CnfExpTests::true_conjoin_exp_destroy() {
  a->disjoin(2);
  a->conjoin(3); // a = (1 2) (3)
  
  CnfExp x(true);
  CPPUNIT_ASSERT(x.alwaysTrue());
  x.conjoin(a, true); a = NULL;
  CPPUNIT_ASSERT(!x.alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(1, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(3, x.litSize());
  CPPUNIT_ASSERT_EQUAL(2, x.clauseSize());
  ASSERT_CONTAINS(&x, 1, 2);
  ASSERT_CONTAINS(&x, 3);
}    

void CnfExpTests::exp_conjoin_true() {
  a->disjoin(2);
  a->conjoin(3); // a = (1 2) (3)
  
  CnfExp x(true);
  CPPUNIT_ASSERT(x.alwaysTrue());
  a->conjoin(&x, false);
  x.conjoin(a, false);
  CPPUNIT_ASSERT(!a->alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(1, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(3, a->litSize());
  CPPUNIT_ASSERT_EQUAL(2, a->clauseSize());
  ASSERT_CONTAINS(a, 1, 2);
  ASSERT_CONTAINS(a, 3);
}

void CnfExpTests::false_conjoin_exp_nodestroy() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  x.conjoin(a, false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(0, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, x.litSize());
  CPPUNIT_ASSERT_EQUAL(0, x.clauseSize());

  ASSERT_CONTAINS(a, 1, 3);
  ASSERT_CONTAINS(a, 2, 3);
}

void CnfExpTests::false_conjoin_exp_destroy() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  x.conjoin(a, true); a = NULL;
  CPPUNIT_ASSERT(x.alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(0, x.singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, x.litSize());
  CPPUNIT_ASSERT_EQUAL(0, x.clauseSize());
}

void CnfExpTests::exp_conjoin_false() {
  a->conjoin(2);
  a->disjoin(3); // a = (1 3) (2 3)
  
  CnfExp x(false);
  CPPUNIT_ASSERT(x.alwaysFalse());
  a->conjoin(&x, false);
  CPPUNIT_ASSERT(a->alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(0, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, a->litSize());
  CPPUNIT_ASSERT_EQUAL(0, a->clauseSize());
}

void CnfExpTests::singleton_becomes_true() {
  a->disjoin(-1);
#if MERGING > 0
  CPPUNIT_ASSERT(a->alwaysTrue());
  CPPUNIT_ASSERT_EQUAL(0, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, a->litSize());
  CPPUNIT_ASSERT_EQUAL(1, a->clauseSize());
#endif
}

void CnfExpTests::singleton_becomes_false() {
  a->conjoin(-1);
#if MERGING > 0
  CPPUNIT_ASSERT(a->alwaysFalse());
  CPPUNIT_ASSERT_EQUAL(0, a->singletons().size());
  CPPUNIT_ASSERT_EQUAL(0, a->litSize());
  CPPUNIT_ASSERT_EQUAL(0, a->clauseSize());
#endif
}
