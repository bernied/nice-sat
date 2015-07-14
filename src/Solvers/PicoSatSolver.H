#ifndef NICESAT_SOLVERS_PICO_SAT_SOLVER
#define NICESAT_SOLVERS_PICO_SAT_SOLVER

#include "Solvers/Solver.H"
#include "Support/Shared.H"

#include <string>
#include <cassert>

extern "C" {
  #include "picosat.h"
}

using namespace std;

namespace nicesat {
  class CnfExp;
  
  class PicoSatSolver : public Solver {
  public:
    PicoSatSolver() { picosat_init(); }
    ~PicoSatSolver() { picosat_reset(); }

    Answer runSolver() { return (Answer) picosat_sat(-1); }
    
    Assignment deref(Literal lit) { return (Assignment) picosat_deref(lit); }
    Literal getNewVar() { return picosat_inc_max_var(); }
    Literal getVar(Literal lit) { return lit; }
    void reserveLit(Literal lit) {
      picosat_adjust(lit);
    }

    void add(const CnfExp& exp);
    void add(Literal lit) {
      assert(lit != 0);
      picosat_add(lit);
      picosat_add(0);
    }
    
    void constrain(Literal lit, const CnfExp& exp);
  };
}

#endif//NICESAT_SOLVERS_PICO_SAT_SOLVER
