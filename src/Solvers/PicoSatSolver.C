#include "Solvers/PicoSatSolver.H"

#include "ADTs/CnfExp.H"
#include "Support/Shared.H"

#include <unistd.h>

namespace nicesat {
  void PicoSatSolver::add(const CnfExp& exp) {
    if (exp.alwaysFalse()) {
      Literal l = getNewVar();
      picosat_add(l);
      picosat_add(0);
      picosat_add(-l);
      picosat_add(0);
      return;
    } else if (exp.alwaysTrue()) {
      return;
    }
    
    const LitVector& singletons = exp.singletons();
    for (int i = 0; i < singletons.size(); i++) {
      picosat_add(singletons[i]);
      picosat_add(0);
    }

    CnfExp::Clause* iter = exp.head();
    while (iter) {
      const LitVector& clause = iter->_litVec;
      for (int i = 0; i < clause.size(); i++) picosat_add(clause[i]);
      picosat_add(0);
      iter = iter->_next;
    }
  }

  void PicoSatSolver::constrain(Literal lit, const CnfExp& exp) {
    if (exp.alwaysFalse()) {
      picosat_add(-lit);
      picosat_add(0);
      return;
    } else if (exp.alwaysTrue()) {
      return;
    }
    
    const LitVector& singletons = exp.singletons();
    for (int i = 0; i < singletons.size(); i++) {
      picosat_add(-lit);
      picosat_add(singletons[i]);
      picosat_add(0);
    }

    CnfExp::Clause* iter = exp.head();
    while (iter) {
      const LitVector& clause = iter->_litVec;
      picosat_add(-lit);
      for (int i = 0; i < clause.size(); i++) picosat_add(clause[i]);
      picosat_add(0);
      
      iter = iter->_next;
    }
  }
}
