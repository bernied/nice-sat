#include <cppunit/extensions/TestFactoryRegistry.h>
#include "ADTs/EdgeTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(EdgeTests);

#include "ADTs/Edge.H"
using namespace nicesat;

void EdgeTests::sizeofEdge() {
  CPPUNIT_ASSERT(sizeof(Edge) == sizeof(Node*));
}

void EdgeTests::varsNodesAndConstants() {
  Edge posCon(0, false); Edge negCon(0, true);
  Edge posVar(5, false); Edge negVar(7, true);
  Edge posExp((Node*) 512, false);
  Edge negExp((Node*) 512, true);

  CPPUNIT_ASSERT(!posCon.isNode());
  CPPUNIT_ASSERT(posCon.isVar());
  CPPUNIT_ASSERT(posCon.isPos());
  CPPUNIT_ASSERT(!posCon.isNeg());
  CPPUNIT_ASSERT(posCon.getVar() == 0);
  CPPUNIT_ASSERT(posCon.isConst());
  CPPUNIT_ASSERT(posCon.isTrue());
  CPPUNIT_ASSERT(!posCon.isFalse());

  CPPUNIT_ASSERT(!negCon.isNode());
  CPPUNIT_ASSERT(negCon.isVar());
  CPPUNIT_ASSERT(!negCon.isPos());
  CPPUNIT_ASSERT(negCon.isNeg());
  CPPUNIT_ASSERT(negCon.getVar() == 0);
  CPPUNIT_ASSERT(negCon.isConst());
  CPPUNIT_ASSERT(!negCon.isTrue());
  CPPUNIT_ASSERT(negCon.isFalse());

  CPPUNIT_ASSERT(!posVar.isNode());
  CPPUNIT_ASSERT(posVar.isVar());
  CPPUNIT_ASSERT(posVar.isPos());
  CPPUNIT_ASSERT(!posVar.isNeg());
  CPPUNIT_ASSERT(posVar.getVar() == 5);
  CPPUNIT_ASSERT(!posVar.isConst());
  CPPUNIT_ASSERT(!posVar.isTrue());
  CPPUNIT_ASSERT(!posVar.isFalse());

  CPPUNIT_ASSERT(!negVar.isNode());
  CPPUNIT_ASSERT(negVar.isVar());
  CPPUNIT_ASSERT(!negVar.isPos());
  CPPUNIT_ASSERT(negVar.isNeg());
  CPPUNIT_ASSERT(negVar.getVar() == 7);
  CPPUNIT_ASSERT(!negVar.isConst());
  CPPUNIT_ASSERT(!negVar.isTrue());
  CPPUNIT_ASSERT(!negVar.isFalse());

  CPPUNIT_ASSERT(posExp.isNode());
  CPPUNIT_ASSERT(!posExp.isVar());
  CPPUNIT_ASSERT(posExp.isPos());
  CPPUNIT_ASSERT(!posExp.isNeg());
  CPPUNIT_ASSERT(posExp.getNodePtr() == (Node*) 512);
  CPPUNIT_ASSERT(!posExp.isConst());
  CPPUNIT_ASSERT(!posExp.isTrue());
  CPPUNIT_ASSERT(!posExp.isFalse());

  CPPUNIT_ASSERT(negExp.isNode());
  CPPUNIT_ASSERT(!negExp.isVar());
  CPPUNIT_ASSERT(!negExp.isPos());
  CPPUNIT_ASSERT(negExp.isNeg());
  CPPUNIT_ASSERT(negExp.getNodePtr() == (Node*) 512);
  CPPUNIT_ASSERT(!negExp.isConst());
  CPPUNIT_ASSERT(!negExp.isTrue());
  CPPUNIT_ASSERT(!negExp.isFalse());  
}

void EdgeTests::polarity() {
  Edge e1(54, false);
  Edge e2(54, true);
  Edge e3(78, false);
  Edge e4(88, true);

  CPPUNIT_ASSERT(e1.sameNode(e2));    CPPUNIT_ASSERT(e2.sameNode(e1));
  CPPUNIT_ASSERT(!e1.sameNode(e3));   CPPUNIT_ASSERT(!e3.sameNode(e1));
  CPPUNIT_ASSERT(!e1.sameNode(e4));   CPPUNIT_ASSERT(!e4.sameNode(e1));
  CPPUNIT_ASSERT(!e2.sameNode(e3));   CPPUNIT_ASSERT(!e3.sameNode(e2));
  CPPUNIT_ASSERT(!e2.sameNode(e4));   CPPUNIT_ASSERT(!e4.sameNode(e2));
  CPPUNIT_ASSERT(!e3.sameNode(e4));   CPPUNIT_ASSERT(!e3.sameNode(e4));

  CPPUNIT_ASSERT(!e1.sameSign(e2));   CPPUNIT_ASSERT(!e2.sameSign(e1));
  CPPUNIT_ASSERT(e1.sameSign(e3));    CPPUNIT_ASSERT(e3.sameSign(e1));
  CPPUNIT_ASSERT(!e1.sameSign(e4));   CPPUNIT_ASSERT(!e4.sameSign(e1));
  CPPUNIT_ASSERT(!e2.sameSign(e3));   CPPUNIT_ASSERT(!e3.sameSign(e2));
  CPPUNIT_ASSERT(e2.sameSign(e4));    CPPUNIT_ASSERT(e4.sameSign(e2));
  CPPUNIT_ASSERT(!e3.sameSign(e4));   CPPUNIT_ASSERT(!e3.sameSign(e4));

  CPPUNIT_ASSERT(!e1.sameNodeOppSign(e1));   CPPUNIT_ASSERT(!e1.sameNodeOppSign(e1));
  CPPUNIT_ASSERT(e1.sameNodeOppSign(e2));    CPPUNIT_ASSERT(e2.sameNodeOppSign(e1));
  CPPUNIT_ASSERT(!e1.sameNodeOppSign(e3));   CPPUNIT_ASSERT(!e3.sameNodeOppSign(e1));
  CPPUNIT_ASSERT(!e1.sameNodeOppSign(e4));   CPPUNIT_ASSERT(!e4.sameNodeOppSign(e1));
  CPPUNIT_ASSERT(!e2.sameNodeOppSign(e3));   CPPUNIT_ASSERT(!e3.sameNodeOppSign(e2));
  CPPUNIT_ASSERT(!e2.sameNodeOppSign(e4));   CPPUNIT_ASSERT(!e4.sameNodeOppSign(e2));
  CPPUNIT_ASSERT(!e3.sameNodeOppSign(e4));   CPPUNIT_ASSERT(!e3.sameNodeOppSign(e4));

  e1.negate();
  CPPUNIT_ASSERT(e1 == e2);
  e1.abs();
  CPPUNIT_ASSERT(e1 != e2);
  CPPUNIT_ASSERT(e1.sameNode(e2));

  CPPUNIT_ASSERT(~(~e1) == e1);
}

void EdgeTests::copyConstructor() {
  Edge e1;
  Edge e2(e1);
  Edge e3((void*) e1);
  CPPUNIT_ASSERT(e1 == e2);
  CPPUNIT_ASSERT(e1 == e3);
}
