#ifndef NICESAT_NICE_SAT_H_
#define NICESAT_NICE_SAT_H_

#include "ADTs/Edge.H"
#include "ADTs/Node.H"
#include "ADTs/Vec.H"
#include "ADTs/GlobalTable.H"

#include "Passes/StatsPass.H"

#include "Solvers/Solver.H"
#include "Solvers/NullSolver.H"

namespace nicesat {
  /**
   * This class embodies the public interface provided by the NICE SAT
   * library.  For most users, this is the only thing that needs to be
   * looked at.
   */
  class NiceSat {
  public:
    /**
     * If set to true, this parameter causes the construction
     * functions to perform matching, which tries to detect IFFs and
     * ITEs in a conjunction.
     */
    bool          doMatching;

    /**
     * If set to true, this parameter causes various steps to produce
     * .dot files that can be run through GraphViz to visualize the
     * NICE dag in various states.
     */
    bool          doGraphViz;

    /**
     * The default constructor for NiceSat.  Uses a NULL solver,
     * doesn't add the clauses to anything.  Useful for timing the
     * actual CNF generation procedure without the overhead of
     * outputting the CNF.
     */
    NiceSat();

    /**
     * This is the constructor that should be used most often.  It
     * constructs an instance of NiceSat that produces CNF using the
     * given solver.
     * @param solver Reference to the Solver instance to use for CNF
     * production.
     */
    NiceSat(Solver& solver);

    /**
     * Destructor for NiceSat.
     */
    virtual ~NiceSat() { if (_freeSolver) delete _solver; }

    /**
     * Initialize the data structures to handle at least numNodes.
     * Currently, this amounts to ensuring that the Global Table has
     * sufficient capacity.  If reading from an input format where
     * this is easily determined, it can improve efficiency by
     * reducing the number of times the global table needs to grow.
     * @param numNodes an estimate on the number of elements that will
     * need to be in the global table.
     */
    void predictNumNodes(size_t numNodes);

    /**
     * Return the variable corresponding to the CNF literal lit.
     * @pre Requires that the literal has previously been allocated by
     * the solver (eg, through a previous call to createVar or the use
     * of reserveLit).
     * @param lit The desired literal
     * @return An Edge pointing to the NICE variable.
     */
    Edge createVar_(Literal lit) { return Edge(_solver->getVar(lit)); }
    
    /**
     * Create an edge pointing to a fresh NICE variable.
     * @return An Edge pointing to the new NICE variable
     */
    Edge createVar() { return Edge(_solver->getNewVar()); }

    /**
     * Create an edge pointing to the conjunction of the given edges.
     * @param args The arguments to the conjunciton.
     * @return an Edge pointing to the conjunction.
     */
    Edge createAnd(Vec<Edge>& args);

    /**
     * Create an edge pointing to the equivalence (IFF a b)
     * @param a The first argument to the equivalence
     * @param b The second argument to the equivalence
     * @return an Edge pointing to the equivalence
     */
    Edge createIff(Edge a, Edge b);

    /**
     * Create an edge pointing to the If-Then-Else:
     *  (if tst tt ff)
     * @param tst The test expression for the ITE
     * @param tt The true branch of the ITE
     * @param ff The false branch of the ITE
     * @return an Edge pointing to the ITE
     */
    Edge createIte(Edge tst, Edge tt, Edge ff);

    /**
     * Create the conjunction of two arguments.
     * @param a The first argument to the conjunction
     * @param b The second argument to the conjunction
     * @return an Edge pointing to the conjunction
     */
    Edge createAnd(Edge a, Edge b);

    /**
     * Create the disjunction of two arguments.
     * @param a The first argument to the disjunction
     * @param b The second argument to the disjunction
     * @return an Edge pointing to the disjunction
     */
    Edge createOr(Edge a, Edge b);

    /**
     * Create an edge pointing to the disjunction of the arguments
     * @param args The arguments to the disjunction
     * @return an Edge pointing to the disjunction.
     */
    Edge createOr(Vec<Edge>& args);

    /**
     * Create an edge pointing to the implication lhs -> rhs
     * @param lhs The left hand side of the implication
     * @param rhs The right hand side of the implication
     * @return an Edge pointing to the implication.
     */
    Edge createImpl(Edge lhs, Edge rhs);

    /**
     * Add the formula represented by f to the CNF.
     * @param f The literal to add to the CNF
     * @param removable if f might be removed later, than this must be
     * true
     */
    void add (Edge f, bool removable = false);

    /**
     * Invoke the SAT solver.
     * @return the answer produced by the SAT solver.
     */
    Answer solve() { return _solver->runSolver(); }

    /**
     * Determine the value of an Edge.
     * @param e The edge to dereference
     * @return The result of dereferencing the node.  May be
     * Assignment_Unknown if insufficient information exists to
     * determine the value, if the SAT solver hasn't yet been run, or
     * if the SAT solver interface doesn't support dereferencing (eg,
     * NullSolver or FileSolver).
     */
    Assignment deref(Edge e) const;

    /**
     * Print stats about the given NICE dag.
     * @param root The root of the garph to print stats about
     */
    void printGraphStats(Edge root);

    /**
     * Print various statistics that don't depend on a given root.
     * Eg., number of matches
     */
    void printStats();

    /**
     * Return a reference to the SAT solver being interacted with by
     * the current NICESAT interface.
     * @return The solver being used by this instance
     */
    Solver& getSolver() { return *_solver; }

    /**
     * Used to log the start of a pass.  Initializes some state, but
     * otherwise acts as reportf (a version of printf that precedes
     * every line with "c ").
     * @param str The format
     */
    void logStart(const char* str, ...);

    /**
     * Used to log the end of a pass.  Finishes the line started by
     * log start, and displays the amount of time taken by the pass.
     */
    void logEnd();

  private:
    /**
     * The global table that is used for structural hashing all of the
     * nodes created by this NiceSat instance.
     */
    GlobalTable   _gtab;

    /**
     * The work queue used for the DFS in the passes and in various
     * other places.  By reusing it for multiple passes it only has to
     * grow big enough for the tree once, after that no more memory
     * allocation is necessary.
     */
    Vec<Edge>     _workQ;

    /**
     * The start time of the last call to logStart.
     */
    double        _time;

    /**
     * The length of the starting message passed to logStart.
     */
    int           _startMsgLen;

    /**
     * The number of matches that have been found.
     */
    int           _numMatches;

    /**
     * The instance of the statistics pass.  This is kept here, so
     * that if called incrementally, the old counts are not lost.
     */
    StatsPass     _stats;

    /**
     * Pointer to the SAT solver being used.
     */
    Solver*       _solver;

    /**
     * True if the memory used by _solver should be deleted in the
     * destructor.
     */
    bool          _freeSolver;
  };
}

#endif//NICESAT_NICE_SAT_H_
