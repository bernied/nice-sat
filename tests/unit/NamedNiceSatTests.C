#include <cppunit/extensions/TestFactoryRegistry.h>
#include "NamedNiceSatTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(NamedNiceSatTests);

#include "NamedNiceSat.H"
using namespace nicesat;

void NamedNiceSatTests::namedVars() {
  NamedNiceSat<int> niceSat;
  Edge var1 = niceSat.createVar(300);
  Edge var2 = niceSat.createVar(100);
  Edge var3 = niceSat.createVar();
  Edge var4 = niceSat.createVar(200);

  CPPUNIT_ASSERT(var1 != var2);
  CPPUNIT_ASSERT(var1 != var3);
  CPPUNIT_ASSERT(var1 != var4);
  CPPUNIT_ASSERT(var2 != var3);
  CPPUNIT_ASSERT(var2 != var4);
  CPPUNIT_ASSERT(var3 != var4);

  int n;
  CPPUNIT_ASSERT(niceSat.lookupName(var1, n));
  CPPUNIT_ASSERT_EQUAL(300, n);
  CPPUNIT_ASSERT(niceSat.lookupName(var2, n));
  CPPUNIT_ASSERT_EQUAL(100, n);
  CPPUNIT_ASSERT(!niceSat.lookupName(var3, n));
  CPPUNIT_ASSERT(niceSat.lookupName(var4, n));
  CPPUNIT_ASSERT_EQUAL(200, n);

  CPPUNIT_ASSERT(niceSat.createVar(300) == var1);
  CPPUNIT_ASSERT(niceSat.createVar(200) == var4);
  CPPUNIT_ASSERT(niceSat.createVar(100) == var2);
}
