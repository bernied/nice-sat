#ifndef NICESAT_ADTS_LIT_VECTOR_H_
#define NICESAT_ADTS_LIT_VECTOR_H_

#include <cstdlib>
#include <cassert>

namespace nicesat {
  typedef int Literal;

  /**
   * Class for representing a vector of literals.  The operations
   * supported include basic construction, construction from another
   * literal vector and an additional literal (or vector), as well as
   * destructive addition of a literal or vector.  This corresponds
   * directly with the operations performed on the set of singleton
   * clauses, and the representation of clauses.
   */
  class LitVector {
  public:
    /**
     * Constructor for an empty literal vector.
     */
    LitVector();

    /**
     * Copy Constructor for literal vectors.
     * @param other The LitVector to copy.
     */
    LitVector(const LitVector& other);

    /**
     * Initialize a literal vector to contain one literal.  Used for
     * re-initializing after construction (since these are pooled).
     * 
     * @param l The literal to put in the vector
     */
    void init(Literal l);

    /**
     * Initialize a literal vector to contain two literals.  Used for
     * re-initializing after construction (since these are pooled).
     * @pre l1 < l2
     * @param l1 The first literal to put in the vector
     * @param l2 The second literal to put in the vector
     */
    void init(Literal l1, Literal l2);

    /**
     * Initialize a literal vector to contain a copy of another
     * vector.  Used for re-initializing after construction (since
     * these are pooled).
     * @param other The literal vector to copy
     */
    void init(const LitVector& other);

    /**
     * Initialize a literal vector to contain a copy of another vector
     * plus a single literal.  Used for re-initializing after
     * construction (since these are pooled).
     * 
     * @param other The literal vector to copy
     * @param l The literal to add
     */
    void init(const LitVector& other, Literal l);

    /**
     * Initialize a literal vector to contain the contents of two
     * literal vectors Used for re-initializing after construction
     * (since these are pooled).
     * 
     * @param other1 The first literal vector to copy
     * @param other2 The second literal vector to copy
     */
    void init(const LitVector& other1, const LitVector& other2);

    /**
     * Destructor for literal vectors.
     */
    ~LitVector() { free(_lits); }; 

    /**
     * Add a literal to this literal vector.
     * @param l The literal to add
     * @return Reference to this literal vector
     */
    const LitVector& operator+=(Literal l);

    /**
     * Add a literal vector to this literal vector.
     * @param other The literal vector to add
     * @return Reference to this literal vector
     */
    const LitVector& operator+=(const LitVector& other);

    /**
     * Assignment operator
     * @param other The literal vector to assign
     * @return Reference to this literal vector
     */
    const LitVector& operator=(const LitVector& other);

    /**
     * Swap the underlying pointers with another literal vector.
     * @param other The literal vector to swap with.
     */
    void swap(LitVector& other);

    /**
     * Accessor for the size of this literal vector.
     * @return the size of this literal vector.
     */
    int size() const { return _size; }

    /**
     * Assignment operator for the size.  Should only be used for
     * resizing down.
     * @pre size <= _size
     * @param size The new size.
     */
    void setSize(int size) { assert (size <= _size); _size = size; }

    /**
     * Accessor for a literal.
     * @param n The index of the desired literal.
     * @pre n < _size
     * @return The desired literal.
     */
    Literal operator[](int n) const { assert(n < _size); return _lits[n]; }
    
    /**
     * Accessor for a literal.
     * @param n The index of the desired literal.
     * @pre n < _size
     * @return The desired literal.
     */
    Literal& operator[](int n) { assert(n < _size); return _lits[n]; }

    /**
     * Clear this literal vector without freeing memory.
     */
    void clear() { _size = 0; }

  private:
    /**
     * Add a single literal to this literal vector, performing merging
     * as appropriate.
     * @param l The literal to add
     */
    void addLit(Literal l);

    /**
     * Add a literal vector to this literal vector, performing
     * merging.
     * @pre The contents of this literal vector has been moved to
     * _lits + other._size, and buffer has been enlarged to hold _size
     * + other._size literals.
     * @param other The literal vector to add
     */
    void addVector(const LitVector& other);

    /**
     * Add a literal vector to this literal vector, performing
     * merging.  This is a special case of addVector that assumes
     * _size >= MERGING, so it isn't necessary to actually merge the
     * front parts of the two lists, only to perform the actual merge
     * operation.
     * 
     * @pre The contents of this literal vector has been moved to
     * _lits + other._size, and buffer has been enlarged to hold _size
     * + other._size literals.  Expects _size >= MERGING.
     * @param lits Pointer to the array of literals to add
     * @param size The number of literals to add from lits
     */
    void addVector_(Literal* lits, int size);

    /**
     * The size (number of literals) that are actually stored in this
     * vector.
     */
    int      _size;

    /**
     * The size of the array that has been allocated for this vector.
     * We maintain the invariant that _alloc >= _size.
     */
    int      _alloc;

    /**
     * Pointer to the actual memory used for storing literals.
     */
    Literal* _lits;
  };
};

#endif//NICESAT_ADTS_LIT_VECTOR_H_
