#ifndef ADTS_EDGE_TESTS_H_
#define ADTS_EDGE_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

class EdgeTests : public CppUnit::TestFixture {
public:
  CPPUNIT_TEST_SUITE(EdgeTests);
  CPPUNIT_TEST(sizeofEdge);
  CPPUNIT_TEST(varsNodesAndConstants);
  CPPUNIT_TEST(polarity);
  CPPUNIT_TEST(copyConstructor);
  CPPUNIT_TEST_SUITE_END();
private:
  void sizeofEdge();
  void varsNodesAndConstants();
  void polarity();
  void copyConstructor();
};

#endif//ADTS_EDGE_TESTS_H_
