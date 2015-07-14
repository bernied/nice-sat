#include <cppunit/extensions/TestFactoryRegistry.h>
#include "ADTs/LitVectorTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(LitVectorTests);

#include "ADTs/LitVector.H"
using namespace nicesat;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef MERGING
#error "Expected MERGING to be defined when compiling __FILE__."
#endif

void LitVectorTests::empty_constructor() {
  LitVector v;
  CPPUNIT_ASSERT_EQUAL(0, v.size());
}

void LitVectorTests::singleton_constructor() {
  LitVector v; v.init(1);
  CPPUNIT_ASSERT_EQUAL(1, v.size());
  CPPUNIT_ASSERT_EQUAL(1, v[0]);
}

void LitVectorTests::copy_constructor() {
  LitVector v1; v1.init(1);
  LitVector v2; v2.init(v1);
  CPPUNIT_ASSERT_EQUAL(1, v2.size());
  CPPUNIT_ASSERT_EQUAL(1, v2[0]);
}

void LitVectorTests::addLit_constructor() {
  LitVector v1; v1.init(1);
  LitVector v2; v2.init(v1, 2);
  CPPUNIT_ASSERT_EQUAL(2, v2.size());
  CPPUNIT_ASSERT_EQUAL(1, v2[0]);
  CPPUNIT_ASSERT_EQUAL(2, v2[1]);
}

void LitVectorTests::join_constructor() {
  LitVector v1; v1.init(1);
  LitVector v2; v2.init(2);
  LitVector v3; v3.init(v1, v2);
  CPPUNIT_ASSERT_EQUAL(2, v3.size());
  CPPUNIT_ASSERT_EQUAL(1, v3[0]);
  CPPUNIT_ASSERT_EQUAL(2, v3[1]);
}

void LitVectorTests::addLit_destroy() {
  LitVector v; v.init(1);
  v += 2;
  v += 3;
  CPPUNIT_ASSERT_EQUAL(3, v.size());
  CPPUNIT_ASSERT_EQUAL(1, v[0]);
  CPPUNIT_ASSERT_EQUAL(2, v[1]);
  CPPUNIT_ASSERT_EQUAL(3, v[2]);
}

void LitVectorTests::join_destroy() {
  LitVector v;  v.init(1);
  LitVector v2; v2.init(2);
  v2 += 3;
  v += v2;
  CPPUNIT_ASSERT_EQUAL(3, v.size());
  CPPUNIT_ASSERT_EQUAL(1, v[0]);
  CPPUNIT_ASSERT_EQUAL(2, v[1]);
  CPPUNIT_ASSERT_EQUAL(3, v[2]);
}

void LitVectorTests::merge_duplicate() {
  LitVector v; v.init(1);
  v += 2;
  LitVector v2; v2.init(2);
  v2 += 3;

  LitVector v3; v3.init(v, v2);
  v += v2;
#if MERGING > 0
  CPPUNIT_ASSERT_EQUAL(3, v.size());
  CPPUNIT_ASSERT_EQUAL(1, v[0]);
  CPPUNIT_ASSERT_EQUAL(2, v[1]);
  CPPUNIT_ASSERT_EQUAL(3, v[2]);

  CPPUNIT_ASSERT_EQUAL(3, v3.size());
  CPPUNIT_ASSERT_EQUAL(1, v3[0]);
  CPPUNIT_ASSERT_EQUAL(2, v3[1]);
  CPPUNIT_ASSERT_EQUAL(3, v3[2]);
#else
  CPPUNIT_ASSERT_EQUAL(4, v.size());
  CPPUNIT_ASSERT_EQUAL(1, v[0]);
  CPPUNIT_ASSERT_EQUAL(2, v[1]);
  CPPUNIT_ASSERT_EQUAL(2, v[2]);
  CPPUNIT_ASSERT_EQUAL(3, v[3]);

  CPPUNIT_ASSERT_EQUAL(4, v3.size());
  CPPUNIT_ASSERT_EQUAL(1, v3[0]);
  CPPUNIT_ASSERT_EQUAL(2, v3[1]);
  CPPUNIT_ASSERT_EQUAL(2, v3[2]);
  CPPUNIT_ASSERT_EQUAL(3, v3[3]);
#endif
}

void LitVectorTests::merge_negative() {
  LitVector v; v.init(1);
  v += 2;
  LitVector v2; v2.init(-2);
  v2 += 3;

  LitVector v3; v3.init(v, v2);
  v += v2;
#if MERGING > 0
  CPPUNIT_ASSERT_EQUAL(0, v.size());
  CPPUNIT_ASSERT_EQUAL(0, v3.size());
#else
  CPPUNIT_ASSERT_EQUAL(4, v.size());
  CPPUNIT_ASSERT_EQUAL(1, v[0]);
  CPPUNIT_ASSERT_EQUAL(2, v[1]);
  CPPUNIT_ASSERT_EQUAL(-2, v[2]);
  CPPUNIT_ASSERT_EQUAL(3, v[3]);

  CPPUNIT_ASSERT_EQUAL(4, v3.size());
  CPPUNIT_ASSERT_EQUAL(1, v3[0]);
  CPPUNIT_ASSERT_EQUAL(2, v3[1]);
  CPPUNIT_ASSERT_EQUAL(-2, v3[2]);
  CPPUNIT_ASSERT_EQUAL(3, v3[3]);
#endif
}
