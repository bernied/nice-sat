#ifndef NICESAT_SOLVERS_SOLVER_H_
#define NICESAT_SOLVERS_SOLVER_H_

#include <stdint.h>

namespace nicesat {
  enum Assignment {
    Assignment_True    =  1,
    Assignment_False   = -1,
    Assignment_Unknown =  0
  };

  enum Answer {
    Answer_Satisfiable   = 10,
    Answer_Unsatisfiable = 20,
    Answer_Unknown       = 0
  };

  class CnfExp;
  typedef int Literal;

  class Solver {
  public:
    virtual ~Solver() { }

    /**
     * Run the solver and return the result.
     */
    virtual Answer     runSolver() = 0;

    /**
     * Dereference the given literal.
     */
    virtual Assignment deref(Literal lit) = 0;

    /**
     * Reserve all literals <= lit (this is used by the various input
     * methods to carve off a slice of literals that they can use).
     * Can only be called once, and if its called it must be before
     * any calls to getNewVar (thus 0...lit are reserved)
     */
    virtual void reserveLit(Literal lit) = 0;

    /**
     * Return a fresh (unused & unreserved) literal
     */
    virtual Literal getNewVar() = 0;

    /**
     * Return the Literal corresponding to the given CNF literal.
     */
    virtual Literal getVar(Literal lit) = 0;

    /**
     * Add the clauses corresponding to exp to the solvers clause DB.
     */
    virtual void add(const CnfExp& exp) = 0;
    virtual void add(Literal lit) = 0;

    /**
     * Add the clauses corresponding to lit -> exp to the solvers
     * clause DB.
     */
    virtual void constrain(Literal lit, const CnfExp& exp) = 0;
  };
}

#endif//NICESAT_SOLVERS_SOLVER_H_
