#ifndef NICESAT_SOLVERS_NULL_SOLVER_H_
#define NICESAT_SOLVERS_NULL_SOLVER_H_

#include "Solvers/Solver.H"

namespace nicesat {
  class NullSolver : public Solver {
  private:
    Literal _varNum;
    
  public:
    NullSolver() : _varNum(0) { }
    ~NullSolver()             { }

    Answer     runSolver()        { return Answer_Unknown;     }
    Assignment deref(Literal lit) { return Assignment_Unknown; }
    Literal getNewVar()           { return Literal(++_varNum); }
    Literal getVar(Literal lit)   { assert(lit <= _varNum); return lit; }
    void reserveLit(Literal lit)  { assert(_varNum == 0); _varNum = lit; }
    void add(const CnfExp& exp) { }
    void add(Literal lit) { }
    void constrain(Literal lit, const CnfExp& exp) { }
  };
}

#endif//NICESAT_SOLVERS_NULL_SOLVER_H_
