#include <cppunit/extensions/TestFactoryRegistry.h>
#include "ADTs/HashTableTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(HashTableTests);

#include "ADTs/HashTable.H"
using namespace nicesat;

void HashTableTests::insertThenGet() {
  HashTable<int, int> tbl;
  int val = 3;
  CPPUNIT_ASSERT(tbl.lookup(5, val) == false);
  tbl.insert(5, 10);
  CPPUNIT_ASSERT(tbl.lookup(5, val) == true);
  CPPUNIT_ASSERT(val == 10);
  CPPUNIT_ASSERT(tbl.lookup(6, val) == false);
}

void HashTableTests::insertTwo() {
  HashTable<int, int> tbl;
  tbl.insert(5, 10);
  tbl.insert(388, 14);

  int val;
  CPPUNIT_ASSERT(tbl.lookup(5, val) == true);
  CPPUNIT_ASSERT(val == 10);
  CPPUNIT_ASSERT(tbl.lookup(38, val) == false);
  CPPUNIT_ASSERT(tbl.lookup(388, val) == true);
  CPPUNIT_ASSERT(val == 14);
}

void HashTableTests::enlarge() {
  HashTable<int, int> tbl;
  for (int i = 0; i <= 3000; i++) {
    tbl.insert(i, i + 10);
  }

  int val;
  CPPUNIT_ASSERT(tbl.lookup(5, val) == true);
  CPPUNIT_ASSERT(val == 15);
  CPPUNIT_ASSERT(tbl.lookup(3000, val) == true);
  CPPUNIT_ASSERT(val == 3010);
  CPPUNIT_ASSERT(tbl.lookup(4000, val) == false);
}
