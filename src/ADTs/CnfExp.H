#ifndef NICESAT_ADTS_CNF_EXP_H_
#define NICESAT_ADTS_CNF_EXP_H_

#include <cstdlib> // for NULL
#include <ostream>
using namespace std;

#include "ADTs/LitVector.H"

namespace nicesat {

  /**
   * Class used for representing CNF.  The specific representation was
   * chosen to allow for detecting some interesting cases, while
   * remaining efficient.
   *
   * - Singleton clauses are stored in a vector, in _singletons.
   * - All other clauses are stored as a linked list of LitVectors
   *   - The list is singly linked, but we store the head and tail
   *     so we can do destructive append efficiently.
   * - We also record the number of lits and number clauses, because
   *   our heuristics need that.
   * 
   * A note about merging -- we perform merging (looking for
   * duplicates and negations) in both the singleton lit vector and
   * the lit vectors used in each clause.  As such we need a way to
   * represent the expression TRUE and the expression FALSE:
   *   - num lits = 0, num clauses = 0 corresponds to FALSE
   *   - num lits = 0, num clauses > 0 corresponds to TRUE
   */
  class CnfExp {
  public:
    /**
     * Construct the CNF expression corresponding to either the
     * constant true or the constant false (depending on the value of
     * isTrue).
     * @param isTrue If true, this creates the constant true,
     * otherwise the constant false.
     */
    CnfExp(bool isTrue = true);

    /**
     * Construct the CNF containing the singleton clause lit.
     * @param lit The literal to add to the CnfExp.
     */
    CnfExp(Literal lit);

    /**
     * Destructor for the CNF expression.
     */
    ~CnfExp() { clear(); }

    /**
     * Print the CnfExp.
     * @param os The stream to print to.
     */
    void print(ostream& os);
    
    /**
     * Modify this expression to be the conjunction of the current
     * contents with the singleton clause containing lit.
     * @param lit The literal to conjoin with
     */
    void conjoin(Literal lit);

    /**
     * Modify this expression to be the conjunction of the current
     * contents with the contents of exp.  Should destroy the contents
     * of exp (and free the memory), if destroy is true.
     * @param exp The expression to conjoin with
     * @param destroy If true, will destroy the CNF exp contained in exp
     */
    void conjoin(CnfExp* exp, bool destroy);
    
    /**
     * Modify this expression to be the disjunction of the current
     * contents with teh singleton clause containing lit.  Amounts to
     * adding lit to every clause.
     * @param lit The literal to disjoin with
     */
    void disjoin(Literal lit);

    /**
     * Modify this expression to be the conjunction of the current
     * contents with the contents of exp.  Should destroy the contents
     * of exp (and free the memory), if destroy is true.
     * @param exp The expression to disjoin with
     * @param destroy If true, will destroy the CNF exp contained in exp
     */
    void disjoin(CnfExp* exp, bool destroy);

    /**
     * Returns true if the CNF expression is the constant false.
     * @return true if the CNF expression is the constant false.
     */
    bool alwaysFalse() const { return _litSize == 0 && _clauseSize == 0; }

    /**
     * Returns true if the CNF expression is the constant true.
     * @return true if the CNF expression is the constant true.
     */
    bool alwaysTrue()  const { return _litSize == 0 && _clauseSize > 0; }

    /**
     * Accessor for the singletons in this CnfExp.
     * @return The literal vector containing the singleton clauses.
     */
    const LitVector& singletons() const { return _singletons; }

    /**
     * Accessor for the number of literals stored in this CnfExp.
     */
    int litSize() const { return _litSize; }

    /**
     * Accessor for the number of clauses stored in this CnfExp.
     */
    int clauseSize() const { return _clauseSize; }

    /**
     * A clause is a vector of literals and a next pointer (linked
     * list node)
     */
    struct Clause {
      /** The next pointer */
      Clause*   _next;
      
      /** The literal vector */
      LitVector _litVec;
      
      /**
       * Constructor for a clause -- sets _next to NULL.
       */
      Clause() : _next(NULL) { }

      /**
       * Constructor for a clause -- sets _next according to the
       * argument.
       * @param next The value to give next
       */
      Clause(Clause* next) : _next(next) { }
    };

    /**
     * Accessor for the head pointer.
     * @return Pointer to the first non-singleton clause in this CnfExp.
     */
    Clause* head() const { return _head; }

    /**
     * Clear this CnfExp, and either become false or true.
     * @param isTrue if true, this becomes the constant true.  if
     * false, it becomes false.
     */
    void clear(bool isTrue = true);
    
  private:
    /**
     * Copy the contents of exp into this expression.  If destroy is
     * true perform as shallow a copy as possible and delete the exp.
     * @param exp The expression to copy
     * @param destroy Can we destroy exp (and thus take control of its
     * memory where possible)
     */
    void copyExp(CnfExp* exp, bool destroy);

    /**
     * Compute the cross product of this expression with exp.
     * @param exp The expression to cross product with.
     */
    void crossProductWith(CnfExp* exp);

    /** The number of literals in this CnfExp */
    int _litSize;

    /** The number of clausesi n this CnfExp */
    int _clauseSize;

    /** The singleton clauses are stored separately to allow merging
        conjunctions */
    LitVector _singletons;

    /** The head pointer */
    Clause* _head;
    /** The tail pointer */
    Clause* _tail;

    /** A static pool of all the allocated clauses */
    static Clause* _pool;

    /**
     * Return a new clause with the given value for next.  Attempt to
     * use clauses from the pool if possible.
     * @param next The initial value for next.
     * @return A clause that isn't used elsewhere.
     */
    Clause* newClause(Clause* next = NULL) {
      if (_pool) {
        Clause* ans =  _pool;
        _pool = _pool->_next;
        ans->_next = next;
        return ans;
      } else {
        return new Clause(next);
      }
    }
  };
};

#endif//NICESAT_ADTS_CNF_EXP_H_
