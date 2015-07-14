#ifndef ADTS_VEC_TESTS_H_
#define ADTS_VEC_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

class VecTests : public CppUnit::TestFixture {
public:
  CPPUNIT_TEST_SUITE(VecTests);
  CPPUNIT_TEST(basicBehavior);
  CPPUNIT_TEST(stackBehavior);
  CPPUNIT_TEST(popFront);
  CPPUNIT_TEST_SUITE_END();

private:
  void basicBehavior();
  void stackBehavior();
  void popFront();
};

#endif//ADTS_VEC_TESTS_H_
