#ifndef ADTS_LIT_VECTOR_TESTS_H_
#define ADTS_LIT_VECTOR_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

class LitVectorTests : public CppUnit::TestFixture {
public:
  CPPUNIT_TEST_SUITE(LitVectorTests);
  CPPUNIT_TEST(empty_constructor);
  CPPUNIT_TEST(copy_constructor);
  CPPUNIT_TEST(singleton_constructor);
  CPPUNIT_TEST(addLit_constructor);
  CPPUNIT_TEST(join_constructor);

  CPPUNIT_TEST(addLit_destroy);
  CPPUNIT_TEST(join_destroy);
  
  CPPUNIT_TEST(merge_duplicate);
  CPPUNIT_TEST(merge_negative);
  
  CPPUNIT_TEST_SUITE_END();

private:
  void empty_constructor();
  void copy_constructor();
  void singleton_constructor();
  void addLit_constructor();
  void join_constructor();

  void addLit_destroy();
  void join_destroy();

  void merge_duplicate();
  void merge_negative();
};

#endif//ADTS_LIT_VECTOR_TESTS_H_
