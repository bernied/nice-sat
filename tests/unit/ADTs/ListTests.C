#include <cppunit/extensions/TestFactoryRegistry.h>
#include "ADTs/ListTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(ListTests);

#include "ADTs/List.H"
using namespace nicesat;

#define LIST_IS_NRANGE(list, n, offset)               \
  do {                                                \
    List<int>::iterator i = list.first();             \
    for (int m = 0; m < n; m++) {                     \
      CPPUNIT_ASSERT(i != list.end());                \
      CPPUNIT_ASSERT_EQUAL(m + 1 + offset, list[i]);  \
      list.next(i);                                   \
    }                                                 \
    CPPUNIT_ASSERT(i == list.end());                  \
  } while (0)

void ListTests::testEmptyListCreation() {
  List<int> list;
  CPPUNIT_ASSERT(list.isEmpty());
  CPPUNIT_ASSERT_EQUAL(0, list.size());
}

void ListTests::testInsertFront() {
  List<int> list;
  list.insertFront(2);
  list.insertFront(1);
  CPPUNIT_ASSERT(!list.isEmpty());
  CPPUNIT_ASSERT_EQUAL(2, list.size());
}

void ListTests::testIteration_NonEmpty() {
  List<int> list;
  list.insertFront(2);
  list.insertFront(1);

  LIST_IS_NRANGE(list, 2, 0);
}

void ListTests::testIteration_Empty() {
  List<int> list;
  List<int>::iterator i = list.first();
  CPPUNIT_ASSERT(i == list.end());
}

void ListTests::testInsertBack() {
  List<int> list;

  list.insertBack(1);
  list.insertBack(2);
  list.insertBack(3);
  
  CPPUNIT_ASSERT(!list.isEmpty());
  CPPUNIT_ASSERT_EQUAL(3, list.size());
  LIST_IS_NRANGE(list, 3, 0);
}

void ListTests::testAppend_NonEmpty() {
  List<int> list1;
  List<int> list2;
  
  list1.insertBack(1);
  list1.insertBack(2);

  list2.insertBack(3);
  list2.insertBack(4);

  list1.append(list2);

  CPPUNIT_ASSERT(!list1.isEmpty());
  CPPUNIT_ASSERT_EQUAL(4, list1.size());
  CPPUNIT_ASSERT(!list2.isEmpty());
  CPPUNIT_ASSERT_EQUAL(2, list2.size());
  LIST_IS_NRANGE(list1, 4, 0);
  LIST_IS_NRANGE(list2, 2, 2);
}

void ListTests::testAppend_EmptyLHS() {
  List<int> list1;
  List<int> list2;
  
  list2.insertBack(3);
  list2.insertBack(4);

  list1.append(list2);

  CPPUNIT_ASSERT(!list1.isEmpty());
  CPPUNIT_ASSERT_EQUAL(2, list1.size());
  CPPUNIT_ASSERT(!list2.isEmpty());
  CPPUNIT_ASSERT_EQUAL(2, list2.size());
    
  LIST_IS_NRANGE(list1, 2, 2);
  LIST_IS_NRANGE(list2, 2, 2);
}

void ListTests::testAppend_EmptyRHS() {
  List<int> list1;
  List<int> list2;
  
  list1.insertBack(1);
  list1.insertBack(2);

  list1.append(list2);

  CPPUNIT_ASSERT(!list1.isEmpty());
  CPPUNIT_ASSERT(2 == list1.size());
  CPPUNIT_ASSERT(list2.isEmpty());
  CPPUNIT_ASSERT(0 == list2.size());
    
  LIST_IS_NRANGE(list1, 2, 0);
  LIST_IS_NRANGE(list2, 0, 0);
}

void ListTests::testAppend_BothEmpty() {
  List<int> list1;
  List<int> list2;
  list1.append(list2);
  CPPUNIT_ASSERT(list1.isEmpty());
  CPPUNIT_ASSERT(0 == list1.size());
  CPPUNIT_ASSERT(list2.isEmpty());
  CPPUNIT_ASSERT(0 == list2.size());
}

void ListTests::testAppendDestroy_NonEmpty() {
  List<int> list1;
  List<int> list2;
  
  list1.insertBack(1);
  list1.insertBack(2);

  list2.insertBack(3);
  list2.insertBack(4);

  list1.appendDestroy(list2);

  CPPUNIT_ASSERT(!list1.isEmpty());
  CPPUNIT_ASSERT_EQUAL(4, list1.size());
  CPPUNIT_ASSERT(list2.isEmpty());
  CPPUNIT_ASSERT_EQUAL(0, list2.size());

  LIST_IS_NRANGE(list1, 4, 0);
  LIST_IS_NRANGE(list2, 0, 2);
}

void ListTests::testAppendDestroy_EmptyLHS() {
  List<int> list1;
  List<int> list2;
  
  list2.insertBack(3);
  list2.insertBack(4);

  list1.appendDestroy(list2);

  CPPUNIT_ASSERT(!list1.isEmpty());
  CPPUNIT_ASSERT_EQUAL(2, list1.size());
  CPPUNIT_ASSERT(list2.isEmpty());
  CPPUNIT_ASSERT_EQUAL(0, list2.size());
    
  LIST_IS_NRANGE(list1, 2, 2);
  LIST_IS_NRANGE(list2, 0, 2);
}

void ListTests::testAppendDestroy_EmptyRHS() {
  List<int> list1;
  List<int> list2;
  
  list1.insertBack(1);
  list1.insertBack(2);

  list1.appendDestroy(list2);

  CPPUNIT_ASSERT(!list1.isEmpty());
  CPPUNIT_ASSERT_EQUAL(2, list1.size());
  CPPUNIT_ASSERT(list2.isEmpty());
  CPPUNIT_ASSERT_EQUAL(0, list2.size());
    
  LIST_IS_NRANGE(list1, 2, 0);
  LIST_IS_NRANGE(list2, 0, 0);
}

void ListTests::testAppendDestroy_ThenInsert() {
  List<int> list1;
  List<int> list2;

  list1.insertBack(2);
  list1.insertBack(3);

  list2.insertBack(4);
  list2.insertBack(5);

  list1.appendDestroy(list2);

  list1.insertFront(1);
  list1.insertBack(6);
  
  list2.insertFront(8);
  list2.insertBack(9);
  CPPUNIT_ASSERT(!list1.isEmpty());
  CPPUNIT_ASSERT_EQUAL(6, list1.size());
  CPPUNIT_ASSERT(!list2.isEmpty());
  CPPUNIT_ASSERT_EQUAL(2, list2.size());

  LIST_IS_NRANGE(list1, 6, 0);
  LIST_IS_NRANGE(list2, 2, 7);
}

void ListTests::testAppendDestroy_BothEmpty() {
  List<int> list1;
  List<int> list2;
  list1.appendDestroy(list2);
  CPPUNIT_ASSERT(list1.isEmpty());
  CPPUNIT_ASSERT(0 == list1.size());
  CPPUNIT_ASSERT(list2.isEmpty());
  CPPUNIT_ASSERT_EQUAL(0, list2.size());
}
  
void ListTests::testSingletonListCreation1() {
  List<int> list(2);
  CPPUNIT_ASSERT(!list.isEmpty());
  CPPUNIT_ASSERT_EQUAL(1, list.size());
}

void ListTests::testSingletonListCreation2() {
  List<int> list(2);
  list.insertBack(3);
  list.insertFront(1);

  CPPUNIT_ASSERT(!list.isEmpty());
  CPPUNIT_ASSERT_EQUAL(3, list.size());
  LIST_IS_NRANGE(list, 3, 0);
}

void ListTests::testCopyConstructor() {
  List<int> list1;
  list1.insertBack(1);
  list1.insertBack(2);
  list1.insertBack(3);
  list1.insertBack(4);
  list1.insertBack(5);

  List<int> list2(list1);
  CPPUNIT_ASSERT(!list2.isEmpty());
  CPPUNIT_ASSERT_EQUAL(5, list2.size());
  LIST_IS_NRANGE(list2, 5, 0);
}

void ListTests::testAssignment_RhsLonger() {
  List<int> list1;
  list1.insertBack(1);
  list1.insertBack(2);
  list1.insertBack(3);
  list1.insertBack(4);
  list1.insertBack(5);

  List<int> list2;
  list2.insertBack(2);
  list2.insertBack(3);
  
  list2 = list1;
  CPPUNIT_ASSERT_EQUAL(5, list2.size());
  CPPUNIT_ASSERT(!list2.isEmpty());
  LIST_IS_NRANGE(list2, 5, 0);
}

void ListTests::testAssignment_LhsLonger() {
  List<int> list1;
  List<int> list2;
  
  list1.insertBack(1);
  list1.insertBack(2);
  list1.insertBack(3);
  list1.insertBack(4);
  list1.insertBack(5);

  list2.insertBack(1);
  list2.insertBack(2);
  list2.insertBack(3);
  
  list1 = list2;
  CPPUNIT_ASSERT_EQUAL(3, list1.size());
  CPPUNIT_ASSERT(!list1.isEmpty());
  LIST_IS_NRANGE(list1, 3, 0);
}

void ListTests::testAssignment_Equal() {
  List<int> list1;
  list1.insertBack(1);
  list1.insertBack(2);

  List<int> list2;
  list2.insertBack(2);
  list2.insertBack(3);
  
  list1 = list2;
  CPPUNIT_ASSERT_EQUAL(2, list1.size());
  LIST_IS_NRANGE(list1, 2, 1);
}

void ListTests::testAssignment_RhsEmpty() {
  List<int> list1;
  list1.insertBack(1);
  list1.insertBack(2);

  List<int> list2;
  
  list1 = list2;
  CPPUNIT_ASSERT_EQUAL(0, list1.size());
  CPPUNIT_ASSERT(list1.isEmpty());
  LIST_IS_NRANGE(list1, 0, 0);
}

void ListTests::testAssignment_LhsEmpty() {
  List<int> list1;
  list1.insertBack(1);
  list1.insertBack(2);

  List<int> list2;
  
  list2 = list1;
  CPPUNIT_ASSERT_EQUAL(2, list2.size());
  CPPUNIT_ASSERT(!list2.isEmpty());
  LIST_IS_NRANGE(list2, 2, 0);
}

void ListTests::testEquality() {
  List<int> list1;
  List<int> list2;

  list1.insertBack(1);
  list1.insertBack(2);
  
  list2.insertBack(1);
  list2.insertBack(2);

  CPPUNIT_ASSERT(list1 == list2);
  CPPUNIT_ASSERT(!(list1 != list2));

  list2.insertBack(3);
  CPPUNIT_ASSERT(!(list1 == list2));
  CPPUNIT_ASSERT(list1 != list2);
}

void ListTests::testRemove_Middle() {
  List<int> list1;
  list1.insertBack(1);
  list1.insertBack(1);
  list1.insertBack(2);
  list1.insertBack(3);

  List<int>::iterator iter = list1.first();
  list1.next(iter);
  list1.remove(iter, list1.first());
  CPPUNIT_ASSERT_EQUAL(2, list1[iter]);
  CPPUNIT_ASSERT_EQUAL(3, list1.size());
  LIST_IS_NRANGE(list1, 3, 0);
}

void ListTests::testRemove_Head() {
  List<int> list1;
  list1.insertBack(0);
  list1.insertBack(1);
  list1.insertBack(2);
  list1.insertBack(3);

  List<int>::iterator iter = list1.first();
  list1.remove(iter, NULL);
  CPPUNIT_ASSERT_EQUAL(1, list1[iter]);
  CPPUNIT_ASSERT_EQUAL(3, list1.size());
  LIST_IS_NRANGE(list1, 3, 0);
}

void ListTests::testRemove_Tail() {
  List<int> list1;
  list1.insertBack(0);
  list1.insertBack(1);
  list1.insertBack(3);

  List<int>::iterator iter = list1.first();
  list1.next(iter);
  List<int>::iterator prev = iter;
  list1.next(iter);
  list1.remove(iter, prev);
  list1.insertBack(2);
  CPPUNIT_ASSERT(iter == list1.end());
  CPPUNIT_ASSERT_EQUAL(3, list1.size());
  LIST_IS_NRANGE(list1, 3, -1);
}

void ListTests::testCopyAppendBug() {
  List<int> list1;
  list1.insertBack(0);

  List<int> list2;
  list2.insertBack(1);

  List<int> list1a(list1);
  List<int> list2a(list2);
  list1a.append(list2a);
  CPPUNIT_ASSERT_EQUAL(2, list1a.size());
  LIST_IS_NRANGE(list1a, 2, -1);
}
