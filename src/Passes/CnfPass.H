#ifndef NICESAT_PASSES_OUTPUT_CNF_H_
#define NICESAT_PASSES_OUTPUT_CNF_H_

#include "ADTs/Edge.H"
#include "ADTs/Vec.H"
#include "Solvers/Solver.H"

namespace nicesat {
  /**
   * Class containing all the code for the CNF production algorithm.
   */
  class CnfPass {
  public:
    /**
     * Constructor for the CnfPass.
     */
    CnfPass()  { }

    /**
     * Destructor for the CnfPass.
     */
    ~CnfPass() { }

    /**
     * Entry function for the CnfPass.
     * @pre The CountAndExpandPass has been run on e to perform share
     * counting and pseudo-expansion.
     * @param solver the Solver to produce CNF to
     * @param root The edge to produce CNF for
     * @param backtrackLit True if we need to support the eventual
     * removal of e
     * @param workQ a vector to use as the work queue
     */
    void operator()(Solver& solver, Edge root, bool backtrackLit, Vec<Edge>& workQ);

  private:
    /**
     * Fills the argument arrays with the arguments from edge e
     * @param e Edge to use as the source for the argument arrays.
     * @param isNeg The polarity to expand in
     * @param largestEdge Reference to memory to be filled with the
     * largest edge.
     * @return Pointer to the largest destructible CNF, or NULL if
     * none found.
     */
    CnfExp* fillArgs(Edge e, bool isNeg, Edge& largestEdge);
    
    /**
     * Vector to store the arguments we're currently working on
     */
    Vec<Edge> _args;

    /**
     * Introduce a proxy for e in the given polarity.
     * @param solver Reference to the solver (used for getting a fresh
     * literal if necessary)
     * @param e Pointer to the node we're introducing a proxy for.
     * Used to check if a literal has been introduced already.  Also
     * used to record the annotation remembering the literal.
     * @param exp Pointer to the CnfExp that is receiving the proxy.
     * This CnfExp is deleted by introProxy.
     * @param isNeg The polarity
     * @return The literal that was introduced
     */
    Literal introProxy(Solver& solver, Edge e, CnfExp* exp, bool isNeg);

    /**
     * Return the literal that is encoded in the CnfExp pointer.
     * @pre ptr is actually a proxy (the 1-bit is set to 1)
     * @param ptr The pointer to the CnfExp
     * @return The literal encoded in the CnfExp
     */
    inline Literal getProxy(CnfExp* ptr) const {
      assert(isProxy(ptr));
      assert(((Literal) (intptr_t) ptr >> 1) != 0);
      return ((Literal) (intptr_t) ptr >> 1);
    }

    /**
     * Returns true if the pointer encodes a literal(proxy)
     * @param ptr The pointer to check for a proxy
     * @return true if the pointer encodes a proxy
     */
    inline bool isProxy(CnfExp* ptr) const {
      return (bool) ((intptr_t) ptr & 1);
    }

    /**
     * Return the literal for the edge e
     * @pre e is a variable
     * @param e The edge pointing to a variable
     */
    Literal atomLit(Edge e) const { return e.getVar() * e.getPol(); }

    /**
     * Produce the CNF for the conjunction (positive polarity) of the
     * node pointed to by e.
     * @param solver The solver that CNF is being added to
     * @param e The edge pointing to the node that needs CNF
     * @return The CNF representing the positive occurrence of e
     */
    CnfExp* produceConjunction(Solver& solver, Edge e);

    /**
     * Produce the CNF for the disjunction (negative polarity) of the
     * node pointed to by e.
     * @param solver The solver that CNF is being added to
     * @param e The edge pointing to the node that needs CNF
     * @return The CNF representing the negative occurrence of e
     */
    CnfExp* produceDisjunction(Solver& solver, Edge e);

    /**
     * Smart push -- only push nodes that haven't already had CNF
     * produced.  If e is an ITE or IFF, pushes the appropriate
     * pseudo-expansions instead.
     * @param workQ the queue to push onto
     * @param e The edge to push
     */
    void push(Vec<Edge>& workQ, Edge e);

    /**
     * Produce CNF (visit up) the node pointe to by e.
     * @param solver The solver that CNF is being produced to
     * @param e The edge to produce CNF for
     */
    void produceCNF(Solver& solver, Edge e);

    /**
     * Propagate constants from src to dest.
     * @param solver The solver being used
     * @param dest The destination of constant propagation
     * @param src  The source of the constant propagation
     * @param negate Whether the constant in src should be negated
     * before propagation
     * @return True if something was propagated to dest
     */
    bool propagate(Solver& solver, CnfExp*& dest, CnfExp* src, bool negate);

    /**
     * Save the CNF for the given edge, sign to the pointer
     * annotation.  Applies the various variable heuristics as
     * necessary.
     * @param solver The solver being used
     * @param exp The CNF expression
     * @param e The edge being annotated
     * @param sign true if saving the negative CNF
     * @return True if something was propagated to dest
     */
    void saveCNF(Solver& solver, CnfExp* exp, Edge e, bool sign);
  };
}

#endif//NICESAT_PASSES_OUTPUT_CNF_H_
