#include "Support/TestHelpers.H"
#include <cppunit/extensions/HelperMacros.h>

#include "ADTs/Edge.H"
#include "ADTs/Node.H"
#include "Support/Shared.H"
using namespace nicesat;

void checkEdge(const char* file, int line,
               Edge e, bool isNeg, NodeOp op,
               size_t numArgs, Edge args[], bool permute) {
  char* buffer = nsprintf("%s:%d: Invalid edge, assertion failed", file, line);
  CPPUNIT_ASSERT_MESSAGE(buffer, e.isNeg() == isNeg);
  CPPUNIT_ASSERT_MESSAGE(buffer, e.isNode());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(buffer, (int) numArgs, (int) e->size());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(buffer, (int) op, (int) e->op());

  if (permute) {
    HashTable<Edge, bool> seen;
    for (size_t i = 0; i < numArgs; i++) {
      seen.insert((*e)[i], true);
    }

    for (size_t i = 0; i < numArgs; i++) {
      bool expected = false;
      CPPUNIT_ASSERT_MESSAGE(buffer, seen.lookup(args[i], expected) == true);
      CPPUNIT_ASSERT_MESSAGE(buffer, expected);
      seen.insert(args[i], false);
    }
  } else {
    for (size_t i = 0; i < numArgs; i++) {
      CPPUNIT_ASSERT_MESSAGE(buffer, args[i] == (*e)[i]);
    }
  }
  free(buffer);
}
