#include <cppunit/extensions/TestFactoryRegistry.h>
#include "NiceSatTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(NiceSatTests);

void NiceSatTests::setUp() {
  var1 = niceSat.createVar();
  var2 = niceSat.createVar();
  var3 = niceSat.createVar();
}

void NiceSatTests::tearDown() { }

