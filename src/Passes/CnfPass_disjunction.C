#include "Passes/CnfPass.H"
#include "ADTs/CnfExp.H"
#include "ADTs/Node.H"

// Upper bound on the number of clauses allowed in a clause
// This prevents degenerate cases from occurring in the
// produceDisjunct code
#define CLAUSE_MAX 3

namespace nicesat {
  CnfExp* CnfPass::produceDisjunction(Solver& solver, Edge e) {
    Edge largestEdge;
    CnfExp* accum = fillArgs(e, true, largestEdge);
    if (accum == NULL) accum = new CnfExp(false);

    // This is necessary to check to make sure that we don't start out
    // with an accumulator that is "too large".

    /// @todo Strictly speaking, introProxy doesn't *need* to free
    /// memory, then this wouldn't have to reallocate CnfExp

    /// @todo When this call to introProxy is made, the semantic
    /// negation pointer will have been destroyed.  Thus, it will not
    /// be possible to use the correct proxy.  That should be fixed.

    // at this point, we will either have NULL, or a destructible expression
    if (accum->clauseSize() > CLAUSE_MAX)
      accum = new CnfExp(introProxy(solver, largestEdge, accum, largestEdge.isNeg()));

    int i = _args.size();
    while (i != 0) {
      Edge arg(_args[--i]);
      if (arg.isVar()) {
        accum->disjoin(atomLit(arg));
      } else {
        CnfExp* argExp = (CnfExp*) arg->ptrAnnot(arg.isNeg());
        assert(argExp != NULL);

        bool destroy = (--arg->intAnnot(arg.isNeg()) == 0);
        if (isProxy(argExp)) { // variable has been introduced
          accum->disjoin(getProxy(argExp));
        } else if (argExp->litSize() == 0) {
          accum->disjoin(argExp, destroy);
        } else {
          // check to see if we should introduce a proxy
          int aL = accum->litSize();      // lits in accum
          int eL = argExp->litSize();     // lits in argument
          int aC = accum->clauseSize();   // clauses in accum
          int eC = argExp->clauseSize();  // clauses in argument

          if (eC > CLAUSE_MAX || (eL * aC + aL * eC > eL + aC + aL + aC)) {
            accum->disjoin(introProxy(solver, arg, argExp, arg.isNeg()));
          } else {
            accum->disjoin(argExp, destroy);
            if (destroy) arg->ptrAnnot(arg.isNeg()) = NULL;
          }
        }
      }
    }
    
    return accum;
  }
}
