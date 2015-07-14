#include "NiceSatTests.H"

#include "NiceSat.H"
using namespace nicesat;

#include "Support/TestHelpers.H"

void NiceSatTests::createVar() {
  CPPUNIT_ASSERT(var1 != var2);
  CPPUNIT_ASSERT(var1 != var3);
  CPPUNIT_ASSERT(var2 != var3);
}

void NiceSatTests::createIff() {
  Edge iff12  = niceSat.createIff(var1, var2);
  
  // Check basic creation behavior
  CHECK_EDGE_PERMUTE(iff12, false, NodeOp_Iff, { var1, var2 } );

  // Check structural hashing
  CPPUNIT_ASSERT(iff12 == niceSat.createIff(var1, var2));
  CPPUNIT_ASSERT(iff12 == niceSat.createIff(var2, var1));

  // Eliminate negations
  CPPUNIT_ASSERT(iff12 == niceSat.createIff(~var1, ~var2));
  CPPUNIT_ASSERT(iff12 == niceSat.createIff(~var2, ~var1));

  // Check behavior with duplicates
  CPPUNIT_ASSERT(niceSat.createIff( var1,  var1) == True);
  CPPUNIT_ASSERT(niceSat.createIff( var1, ~var1) == False);
  CPPUNIT_ASSERT(niceSat.createIff(~var1,  var1) == False);
  CPPUNIT_ASSERT(niceSat.createIff(~var1, ~var1) == True);

  // Various negations
  CPPUNIT_ASSERT(~iff12 == niceSat.createIff(~var1,  var2));
  CPPUNIT_ASSERT(~iff12 == niceSat.createIff( var1, ~var2));
  CPPUNIT_ASSERT(~iff12 == niceSat.createIff(~var2,  var1));
  CPPUNIT_ASSERT(~iff12 == niceSat.createIff( var2, ~var1));

  // Constants appearing in various places
  CPPUNIT_ASSERT(var1 == niceSat.createIff(True, var1));
  CPPUNIT_ASSERT(var1 == niceSat.createIff(var1, True));
  CPPUNIT_ASSERT(True == niceSat.createIff(True, True));

  CPPUNIT_ASSERT(~var1 == niceSat.createIff(False, var1));
  CPPUNIT_ASSERT(~var1 == niceSat.createIff(var1,  False));
  CPPUNIT_ASSERT(True  == niceSat.createIff(False, False));

  CPPUNIT_ASSERT(False == niceSat.createIff(False, True));
  CPPUNIT_ASSERT(False == niceSat.createIff(True, False));
}

void NiceSatTests::createAnd() {
  Vec<Edge> args;
  CPPUNIT_ASSERT(niceSat.createAnd(args) == True);
  args.push(var1);
  CPPUNIT_ASSERT(niceSat.createAnd(args) == var1);
  args.push(var2);
  CPPUNIT_ASSERT(niceSat.createAnd(args) == niceSat.createAnd(var1, var2));
  CPPUNIT_ASSERT(niceSat.createAnd(args) == niceSat.createAnd(var2, var1));
  args.push(var3);

  Edge and123 = niceSat.createAnd(args);
  CHECK_EDGE_PERMUTE(and123, false, NodeOp_And, { var1, var2, var3 });

  // and(v1, v2, False, v3) == False
  args.pop(); args.push(False); args.push(var3);
  CPPUNIT_ASSERT(niceSat.createAnd(args) == False);

  CPPUNIT_ASSERT(niceSat.createAnd(True, True)  == True);
  CPPUNIT_ASSERT(niceSat.createAnd(True, var1)  == var1);
  CPPUNIT_ASSERT(niceSat.createAnd(var1, True)  == var1);
  CPPUNIT_ASSERT(niceSat.createAnd(False, True) == False);
  CPPUNIT_ASSERT(niceSat.createAnd(True, False) == False);

  // Duplicates
  CPPUNIT_ASSERT(niceSat.createAnd(var1, var1) == var1);
  args.clear(); args.push(var1); args.push(var2); args.push(var1);
  CPPUNIT_ASSERT(niceSat.createAnd(args) == niceSat.createAnd(var1, var2));

  // x and ~x in the list
  CPPUNIT_ASSERT(niceSat.createAnd(~var1, var1) == False);
  CPPUNIT_ASSERT(niceSat.createAnd(var1, ~var1) == False);
  
  args.push(~var2);
  CPPUNIT_ASSERT(niceSat.createAnd(args) == False);
  args.clear();
  args.push(var1); args.push(~var2); args.push(var1); args.push(var2);
  CPPUNIT_ASSERT(niceSat.createAnd(args) == False);
}

void NiceSatTests::createOr() {
  Vec<Edge> args;
  args.push(var1); args.push(var2); args.push(var3);
  CHECK_EDGE_PERMUTE(niceSat.createOr(args), true, NodeOp_And, { ~var1, ~var2, ~var3 } );

  CPPUNIT_ASSERT(niceSat.createOr(var1, ~var1) == True);
}

void NiceSatTests::createImpl() {
  CHECK_EDGE_PERMUTE(niceSat.createImpl(var1, var2), true, NodeOp_And, { var1, ~var2 } );
}

void NiceSatTests::createIte() {
  Edge ite123  = niceSat.createIte(var1, var2, var3);
  
  // Check basic creation behavior
  CHECK_EDGE(ite123, false, NodeOp_Ite, { var1, var2, var3 } );

  // Check structural hashing
  CPPUNIT_ASSERT(ite123 == niceSat.createIte(var1, var2, var3));

  // Eliminate negations
  CPPUNIT_ASSERT(ite123  == niceSat.createIte(~var1,  var3,  var2));
  CPPUNIT_ASSERT(~ite123 == niceSat.createIte( var1, ~var2, ~var3));
  CPPUNIT_ASSERT(~ite123 == niceSat.createIte(~var1, ~var3, ~var2));

  // Test constants in various places
  CPPUNIT_ASSERT(var2                           == niceSat.createIte( True,   var2,  var3));
  CPPUNIT_ASSERT(var3                           == niceSat.createIte(False,   var2,  var3));
  CPPUNIT_ASSERT(niceSat.createOr(var1, var3)   == niceSat.createIte(  var1,  True,  var3));
  CPPUNIT_ASSERT(niceSat.createAnd(~var1, var3) == niceSat.createIte(  var1, False,  var3));
  CPPUNIT_ASSERT(niceSat.createOr(~var1, var2)  == niceSat.createIte(  var1,  var2,  True));
  CPPUNIT_ASSERT(niceSat.createAnd(var1, var2)  == niceSat.createIte(  var1,  var2, False));

  // Test duplicates
  CPPUNIT_ASSERT(var2                            == niceSat.createIte( var1,  var2,  var2));
  CPPUNIT_ASSERT(niceSat.createIff(var1, var2)   == niceSat.createIte( var1,  var2, ~var2));
  CPPUNIT_ASSERT(~niceSat.createIff(var1, var2)  == niceSat.createIte( var1, ~var2,  var2));
  CPPUNIT_ASSERT(niceSat.createOr(var1, var2)    == niceSat.createIte( var1,  var1,  var2));
  CPPUNIT_ASSERT(niceSat.createAnd(~var1, var2)  == niceSat.createIte( var1, ~var1,  var2));
  CPPUNIT_ASSERT(niceSat.createAnd(var1, var2)   == niceSat.createIte(~var1,  var1,  var2));
  CPPUNIT_ASSERT(niceSat.createAnd(var1, var2)   == niceSat.createIte( var1,  var2,  var1));
  CPPUNIT_ASSERT(niceSat.createOr(~var1, var2)   == niceSat.createIte( var1,  var2, ~var1));
  CPPUNIT_ASSERT(niceSat.createOr(var1, var2)    == niceSat.createIte(~var1,  var2,  var1));

  // All the same

  CPPUNIT_ASSERT( var1 == niceSat.createIte( var1,  var1,  var1));
  CPPUNIT_ASSERT( True == niceSat.createIte( var1,  var1, ~var1));
  CPPUNIT_ASSERT(False == niceSat.createIte( var1, ~var1,  var1));
  CPPUNIT_ASSERT(~var1 == niceSat.createIte( var1, ~var1, ~var1));
  CPPUNIT_ASSERT( var1 == niceSat.createIte(~var1,  var1,  var1));
  CPPUNIT_ASSERT(False == niceSat.createIte(~var1,  var1, ~var1));
  CPPUNIT_ASSERT( True == niceSat.createIte(~var1, ~var1,  var1));
  CPPUNIT_ASSERT(~var1 == niceSat.createIte(~var1, ~var1, ~var1));
}

void NiceSatTests::matching() {
  niceSat.doMatching = true;

  Edge ans = niceSat.createAnd(~niceSat.createAnd(~var2, var1),
                               ~niceSat.createAnd(~var1, ~var3));
  CHECK_EDGE(ans, false, NodeOp_Ite, { var1, var2, var3 } );

  ans = niceSat.createAnd(~niceSat.createAnd(~var2, var1),
                          ~niceSat.createAnd(~var1, var2));
  CHECK_EDGE_PERMUTE(ans, false, NodeOp_Iff, { var1, var2 } );
}
