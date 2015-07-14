#ifndef NICESAT_NAMED_NICE_SAT_TESTS_H_
#define NICESAT_NAMED_NICE_SAT_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

class NamedNiceSatTests : public CppUnit::TestFixture {
public:
  CPPUNIT_TEST_SUITE(NamedNiceSatTests);
  CPPUNIT_TEST(namedVars);
  CPPUNIT_TEST_SUITE_END();
  
private:
  void namedVars();
};

#endif//NICESAT_NAMED_NICE_SAT_TESTS_H_
