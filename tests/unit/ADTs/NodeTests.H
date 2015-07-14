#ifndef ADTS_NODE_TESTS_H_
#define ADTS_NODE_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ADTs/Edge.H"

class NodeTests : public CppUnit::TestFixture {
public:
  void setUp();
  
  CPPUNIT_TEST_SUITE(NodeTests);
  CPPUNIT_TEST(createBinaryAnd);
  CPPUNIT_TEST(createIff);
  CPPUNIT_TEST(createIte);
  CPPUNIT_TEST(createNaryAnd);
  CPPUNIT_TEST_SUITE_END();

private:
  void createBinaryAnd();
  void createIff();
  void createIte();
  void createNaryAnd();

  nicesat::Edge a;
  nicesat::Edge b;
  nicesat::Edge c;
};

#endif//ADTS_NODE_TESTS_H_
