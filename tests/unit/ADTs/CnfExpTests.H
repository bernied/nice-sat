#ifndef ADTS_CNF_EXP_TESTS_H_
#define ADTS_CNF_EXP_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>
#include "ADTs/CnfExp.H"
using namespace nicesat;

class CnfExpTests : public CppUnit::TestFixture {
public:
  void setUp();
  void tearDown();
  
  CPPUNIT_TEST_SUITE(CnfExpTests);
  CPPUNIT_TEST(construction);
  
  CPPUNIT_TEST(conjoinLit_singles);
  CPPUNIT_TEST(conjoinExp_nodestroy_singles);
  CPPUNIT_TEST(conjoinExp_destroy_singles);
  CPPUNIT_TEST(disjoinLit_singles);

  CPPUNIT_TEST(check_prepareExps);
  CPPUNIT_TEST(conjoinExp_nodestroy_a);
  CPPUNIT_TEST(conjoinExp_nodestroy_b);
  CPPUNIT_TEST(conjoinExp_destroy_a);
  CPPUNIT_TEST(conjoinExp_destroy_b);
  
  CPPUNIT_TEST(disjoinExp_nodestroy_a);
  CPPUNIT_TEST(disjoinExp_nodestroy_b);
  CPPUNIT_TEST(disjoinExp_destroy_a);
  CPPUNIT_TEST(disjoinExp_destroy_b);
  
  CPPUNIT_TEST(disjoinExp_nodestroy_singletons);
  CPPUNIT_TEST(disjoinExp_destroy_singletons);

  CPPUNIT_TEST(true_disjoin_lit);
  CPPUNIT_TEST(false_disjoin_lit);
  CPPUNIT_TEST(true_disjoin_exp_nodestroy);
  CPPUNIT_TEST(true_disjoin_exp_destroy);
  CPPUNIT_TEST(exp_disjoin_true);
  CPPUNIT_TEST(false_disjoin_exp_nodestroy);
  CPPUNIT_TEST(false_disjoin_exp_destroy);
  CPPUNIT_TEST(exp_disjoin_false);

  CPPUNIT_TEST(true_conjoin_lit);
  CPPUNIT_TEST(false_conjoin_lit);
  CPPUNIT_TEST(true_conjoin_exp_nodestroy);
  CPPUNIT_TEST(true_conjoin_exp_destroy);
  CPPUNIT_TEST(exp_conjoin_true);
  CPPUNIT_TEST(false_conjoin_exp_nodestroy);
  CPPUNIT_TEST(false_conjoin_exp_destroy);
  CPPUNIT_TEST(exp_conjoin_false);

  CPPUNIT_TEST(singleton_becomes_true);
  CPPUNIT_TEST(singleton_becomes_false);
  
  CPPUNIT_TEST_SUITE_END();

private:
  CnfExp* a;
  CnfExp* b;

  bool containsClause(CnfExp* exp, Literal* lits, int litSize);
  
  void construction();
  void conjoinLit_singles();
  void conjoinExp_destroy_singles();
  void conjoinExp_nodestroy_singles();
  void disjoinLit_singles();

  void prepareExps();
  void check_prepareExps();

  void conjoinExp_destroy_a();
  void conjoinExp_destroy_b();
  void conjoinExp_nodestroy_a();
  void conjoinExp_nodestroy_b();

  void disjoinExp_destroy_a();
  void disjoinExp_destroy_b();
  void disjoinExp_nodestroy_a();
  void disjoinExp_nodestroy_b();
  
  void disjoinExp_destroy_singletons();
  void disjoinExp_nodestroy_singletons();

  void true_disjoin_lit();
  void lit_disjoin_true();
  void false_disjoin_lit();
  void lit_disjoin_false();
  void true_disjoin_exp_nodestroy();
  void true_disjoin_exp_destroy();
  void exp_disjoin_true();
  void false_disjoin_exp_nodestroy();
  void false_disjoin_exp_destroy();
  void exp_disjoin_false();

  void true_conjoin_lit();
  void lit_conjoin_true();
  void false_conjoin_lit();
  void lit_conjoin_false();
  void true_conjoin_exp_nodestroy();
  void true_conjoin_exp_destroy();
  void exp_conjoin_true();
  void false_conjoin_exp_nodestroy();
  void false_conjoin_exp_destroy();
  void exp_conjoin_false();

  void singleton_becomes_true();
  void singleton_becomes_false();
};

#endif//ADTS_CNF_EXP_TESTS_H_
