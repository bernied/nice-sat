#include "Passes/CnfPass.H"
#include "ADTs/CnfExp.H"
#include "ADTs/Node.H"

namespace nicesat {
  CnfExp* CnfPass::produceConjunction(Solver& solver, Edge e) {
    Edge largestEdge;
    CnfExp* accum = fillArgs(e, false, largestEdge);
    if (accum == NULL) accum = new CnfExp(true);

    int i = _args.size();
    while (i != 0) {
      Edge arg(_args[--i]);
      if (arg.isVar()) {
        accum->conjoin(atomLit(arg));
      } else {
        CnfExp* argExp = (CnfExp*) arg->ptrAnnot(arg.isNeg());
        assert(argExp != NULL);
        
        bool destroy = (--arg->intAnnot(arg.isNeg()) == 0);
        if (isProxy(argExp)) { // variable has been introduced
          accum->conjoin(getProxy(argExp));
        } else {
          accum->conjoin(argExp, destroy);
          if (destroy) arg->ptrAnnot(arg.isNeg()) = NULL;
        }
      }
    }

    return accum;
  }
}
