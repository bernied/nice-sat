#include <cppunit/extensions/TestFactoryRegistry.h>
#include "ADTs/NodeTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(NodeTests);

#include "ADTs/Node.H"
using namespace nicesat;

void NodeTests::setUp() {
  a = Edge(56);
  b = Edge(1282, true);
  c = Edge(98);
}

void NodeTests::createBinaryAnd() {
  Edge edge = Edge(new (2) Node(NodeOp_And, a, b));
  CPPUNIT_ASSERT(NodeOp_And == edge.getNode().op());
  CPPUNIT_ASSERT_EQUAL(2, (int) edge.getNode().size());
  CPPUNIT_ASSERT(a == edge.getNode()[0]);
  CPPUNIT_ASSERT(b == edge.getNode()[1]);
  delete edge.getNodePtr();
}

void NodeTests::createIff() {
  Edge edge = Edge(new (2) Node(NodeOp_Iff, a, b));
  CPPUNIT_ASSERT(NodeOp_Iff == edge.getNode().op());
  CPPUNIT_ASSERT_EQUAL(2, (int) edge.getNode().size());
  CPPUNIT_ASSERT(a == edge.getNode()[0]);
  CPPUNIT_ASSERT(b == edge.getNode()[1]);
  delete edge.getNodePtr();
}

void NodeTests::createIte() {
  Edge edge = Edge(new (3) Node(NodeOp_Ite, a, b, c));
  CPPUNIT_ASSERT(NodeOp_Ite == edge.getNode().op());
  CPPUNIT_ASSERT_EQUAL(3, (int) edge.getNode().size());
  CPPUNIT_ASSERT(a == edge.getNode()[0]);
  CPPUNIT_ASSERT(b == edge.getNode()[1]);
  CPPUNIT_ASSERT(c == edge.getNode()[2]);
  delete edge.getNodePtr();
}

void NodeTests::createNaryAnd() {
  Vec<Edge> args;
  args.push(a);
  args.push(b);
  args.push(c);
  Edge edge = Edge(new (3) Node(NodeOp_And, args));
  CPPUNIT_ASSERT(NodeOp_And == edge.getNode().op());
  CPPUNIT_ASSERT_EQUAL(3, (int) edge.getNode().size());
  CPPUNIT_ASSERT(a == edge.getNode()[0]);
  CPPUNIT_ASSERT(b == edge.getNode()[1]);
  CPPUNIT_ASSERT(c == edge.getNode()[2]);
  delete edge.getNodePtr();
}
