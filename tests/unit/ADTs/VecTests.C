#include <cppunit/extensions/TestFactoryRegistry.h>
#include "ADTs/VecTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(VecTests);

#include "ADTs/Vec.H"
using namespace nicesat;

void VecTests::basicBehavior() {
  Vec<int> x;
  x.push(5);
  x.push(6);
  x.push(7);
  CPPUNIT_ASSERT_EQUAL(3, (int) x.size());
  CPPUNIT_ASSERT_EQUAL(5, x[0]);
  CPPUNIT_ASSERT_EQUAL(5, x.first());
  CPPUNIT_ASSERT_EQUAL(6, x[1]);
  CPPUNIT_ASSERT_EQUAL(7, x[2]);
  CPPUNIT_ASSERT_EQUAL(7, x.last());
}

void VecTests::stackBehavior() {
  Vec<int> x;
  x.push(5);
  x.push(6);
  x.push(7);
  x.pop();
  CPPUNIT_ASSERT_EQUAL(2, (int) x.size());
  x.push(10);
  CPPUNIT_ASSERT_EQUAL(3, (int) x.size());
  CPPUNIT_ASSERT_EQUAL(5, x[0]);
  CPPUNIT_ASSERT_EQUAL(6, x[1]);
  CPPUNIT_ASSERT_EQUAL(10, x[2]);
}

void VecTests::popFront() {
  Vec<int> x;
  x.push(5);
  x.push(8);
  x.push(10);
  x.push(11);
  CPPUNIT_ASSERT_EQUAL(4, (int) x.size());
  x.popFront();
  x.popFront();
  CPPUNIT_ASSERT_EQUAL(2, (int) x.size());
  CPPUNIT_ASSERT_EQUAL(10, x[0]);
  CPPUNIT_ASSERT_EQUAL(11, x[1]);
  x.push(12);
  x.push(13);
  x.push(14);
  CPPUNIT_ASSERT_EQUAL(5, (int) x.size());
  CPPUNIT_ASSERT_EQUAL(10, x[0]);
  CPPUNIT_ASSERT_EQUAL(11, x[1]);
  CPPUNIT_ASSERT_EQUAL(12, x[2]);
  CPPUNIT_ASSERT_EQUAL(13, x[3]);
  CPPUNIT_ASSERT_EQUAL(14, x[4]);
}
