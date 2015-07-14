#ifndef ADTS_HASH_TABLE_TESTS_H_
#define ADTS_HASH_TABLE_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

class HashTableTests : public CppUnit::TestFixture {
public:
  CPPUNIT_TEST_SUITE(HashTableTests);
  CPPUNIT_TEST(insertThenGet);
  CPPUNIT_TEST(insertTwo);
  CPPUNIT_TEST(enlarge);
  CPPUNIT_TEST_SUITE_END();
  
private:
  void insertThenGet();
  void insertTwo();
  void enlarge();
};

#endif//ADTS_HASH_TABLE_TESTS_H_

