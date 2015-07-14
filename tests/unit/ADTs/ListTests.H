#ifndef NICESAT_ADTS_LIST_TESTS_HPP_
#define NICESAT_ADTS_LIST_TESTS_HPP_

#include <cppunit/extensions/HelperMacros.h>

class ListTests : public CppUnit::TestFixture {
public:
  CPPUNIT_TEST_SUITE(ListTests);
  CPPUNIT_TEST(testEmptyListCreation);
  CPPUNIT_TEST(testSingletonListCreation1);
  CPPUNIT_TEST(testInsertFront);
  CPPUNIT_TEST(testIteration_NonEmpty);
  CPPUNIT_TEST(testSingletonListCreation2);
  CPPUNIT_TEST(testIteration_Empty);
  CPPUNIT_TEST(testInsertBack);
  CPPUNIT_TEST(testAppend_NonEmpty);
  CPPUNIT_TEST(testAppend_EmptyLHS);
  CPPUNIT_TEST(testAppend_EmptyRHS);
  CPPUNIT_TEST(testAppend_BothEmpty);
  
  CPPUNIT_TEST(testCopyAppendBug);
  
  CPPUNIT_TEST(testAppendDestroy_NonEmpty);
  CPPUNIT_TEST(testAppendDestroy_EmptyLHS);
  CPPUNIT_TEST(testAppendDestroy_EmptyRHS);
  CPPUNIT_TEST(testAppendDestroy_BothEmpty);

  CPPUNIT_TEST(testAppendDestroy_ThenInsert);


  CPPUNIT_TEST(testCopyConstructor);
  CPPUNIT_TEST(testAssignment_RhsLonger);
  CPPUNIT_TEST(testAssignment_LhsLonger);
  CPPUNIT_TEST(testAssignment_Equal);
  CPPUNIT_TEST(testAssignment_RhsEmpty);
  CPPUNIT_TEST(testAssignment_LhsEmpty);

  CPPUNIT_TEST(testEquality);

  CPPUNIT_TEST(testRemove_Middle);
  CPPUNIT_TEST(testRemove_Head);
  CPPUNIT_TEST(testRemove_Tail);
  CPPUNIT_TEST_SUITE_END();
  
private:
  void testEmptyListCreation();
  void testSingletonListCreation1();
  void testSingletonListCreation2();
  void testInsertFront();

  void testIteration_NonEmpty();
  void testIteration_Empty();
  void testInsertBack();
  
  void testAppend_NonEmpty();
  void testAppend_EmptyLHS();
  void testAppend_EmptyRHS();
  void testAppend_BothEmpty();

  void testCopyAppendBug();

  void testAppendDestroy_NonEmpty();
  void testAppendDestroy_EmptyLHS();
  void testAppendDestroy_EmptyRHS();
  void testAppendDestroy_BothEmpty();
  void testAppendDestroy_ThenInsert();

  void testCopyConstructor();
  void testAssignment_RhsLonger();
  void testAssignment_LhsLonger();
  void testAssignment_Equal();
  void testAssignment_RhsEmpty();
  void testAssignment_LhsEmpty();

  void testEquality();

  void testRemove_Middle();
  void testRemove_Head();
  void testRemove_Tail();
};
  
#endif//NICESAT_ADTS_LIST_TESTS_HPP_
