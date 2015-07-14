#ifndef NICESAT_SUPPORT_TEST_HELPERS_H_
#define NICESAT_SUPPORT_TEST_HELPERS_H_

#include "ADTs/Edge.H"
#include "ADTs/Node.H"

void checkEdge(const char* file, int line,
               nicesat::Edge e, bool isNeg, nicesat::NodeOp op,
               size_t numArgs, nicesat::Edge args[], bool permute);

#define CHECK_EDGE(e, isNeg, op, ...)                                  \
  do {                                                                 \
    Edge expectedArgs[] = __VA_ARGS__ ;                                \
    checkEdge(__FILE__, __LINE__, (e), (isNeg), (op),                  \
              sizeof(expectedArgs) / sizeof(expectedArgs[0]),          \
              expectedArgs, false);                                    \
  } while(false)

#define CHECK_EDGE_PERMUTE(e, isNeg, op, ...)                          \
  do {                                                                 \
    Edge expectedArgs[] = __VA_ARGS__;                                 \
    checkEdge(__FILE__, __LINE__, (e), (isNeg), (op),                  \
              sizeof(expectedArgs) / sizeof(expectedArgs[0]),          \
              expectedArgs, true);                                     \
  } while(false)
#endif//NICESAT_SUPPORT_TEST_HELPERS_H_

