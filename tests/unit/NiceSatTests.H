#ifndef NICESAT_NICE_SAT_TESTS_H_
#define NICESAT_NICE_SAT_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ADTs/Edge.H"

#include "NiceSat.H"

class NiceSatTests : public CppUnit::TestFixture {
public:
  void setUp();
  void tearDown();
  
  CPPUNIT_TEST_SUITE(NiceSatTests);

  // Basic/Construction Tests
  CPPUNIT_TEST(createVar);
  CPPUNIT_TEST(createIff);
  CPPUNIT_TEST(createAnd);
  CPPUNIT_TEST(createOr);
  CPPUNIT_TEST(createImpl);
  CPPUNIT_TEST(createIte);

  CPPUNIT_TEST(matching);
  
  CPPUNIT_TEST_SUITE_END();

private:
  nicesat::Edge     var1;
  nicesat::Edge     var2;
  nicesat::Edge     var3;
  nicesat::NiceSat  niceSat;
  
  void createVar();
  void createIff();
  void createAnd();
  void createOr();
  void createImpl();
  void createIte();

  void matching();
};

#endif//NICESAT_NICE_SAT_TESTS_H_
