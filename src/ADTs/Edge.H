#ifndef NICESAT_ADTS_EDGE_H_
#define NICESAT_ADTS_EDGE_H_

#include <cassert>
#include <stdint.h>
#include <stdlib.h>

#include "Support/Shared.H"

namespace nicesat {
  class Node;

  /**
   * The Edge serves as a smart-pointer around a Node.  The primary
   * purpose is to allow storing extra information in the low-order
   * bits, such as:
   *   1) Is this a pointer to a node or variable
   *   2) Is this negated
   * If it is a node, then the high-order bits constitute a pointer.
   * If it is a variable, then shifting the high-order bits two spaces
   * to the right will yield an integer, corresponding to the
   * variable.
   *
   * The methods are various mechanisms for making interaction with
   * this smart pointer as seamless as possible.
   */
  class Edge {
  private:
    /**
     * This should be the only field in an Edge.  It is a single
     * pointer worth of information.  The two low-order bits are used
     * for storing information, as follows:
     *   1-bit: If set to 1, this edge is negated, otherwise it is
     *          positive.
     *   2-bit: If set to 1, this edge is a constant or variable,
     *          otherwise it is a node pointer.
     * An edge is a constant if the 2-bit is set to 1, and the rest
     * of the bits are zero (eg, it is the variable 0), otherwise it
     * is whatever variable number is encoded in the high-order bits.
     */
    Node* _node;
    
  public:
    /**
     * Constructor for an edge.  Basically, creates a NULL pointer.
     */
    Edge()              : _node(NULL)      { }

    /**
     * Copy constructor for an Edge.
     * @param e The edge to copy.
     */
    Edge(const Edge& e) : _node(e._node)   { }

    /**
     * Constructor for an edge from a void pointer.  Primarily used
     * for creating an edge from the pointer annotation in a Node.
     * @param v The void* to initialize _node from.
     */
    Edge(void* v)       : _node((Node*) v) { }

    /**
     * Create an edge that is a variable (or constant if var == 0)
     * @param var The variable that the edge should encode (if 0, this
     * is a constant).
     * @param isNeg Whether or not the edge should be negated.
     */
    explicit Edge(int var, bool isNeg = false)
      : _node((Node*) ((uintptr_t) (var << 2) | 2 | (isNeg & 1))) { }

    /**
     * Create an edge that points to a node
     * @param node The node that the edge should point to
     * @param isNeg Whether or not the edge should be negated.
     */
    explicit Edge(Node* node, bool isNeg = false)
      : _node((Node*) (((uintptr_t) node | (isNeg & 1)))) { }

    /**
     * Assignment operator for edges.
     * @param e The edge to copy
     * @return Reference to this edge.
     */
    inline const Edge& operator=(const Edge& e)       { _node = e._node; return *this; }

    /**
     * Inequality comparison operator for edges.
     * @param e The edge to compare to
     * @return True if this edge is not equal to e
     */
    inline bool operator!=(const Edge& e) const { return _node != e._node; }

    /**
     * Equality comparison operator for edges.
     * @param e The edge to compare to
     * @return True if this edge is equal to e
     */
    inline bool operator==(const Edge& e) const { return _node == e._node; }

    /**
     * Less than or equal to comparison operator for edges.
     * @param e The edge to compare to
     * @return True if this edge is less than or equal to e
     */
    inline bool operator<=(const Edge& e) const { return _node <= e._node; }

    /**
     * Greater than or equal to comparison operator for edges.
     * @param e The edge to compare to
     * @return True if this edge is greater than or equal to e
     */
    inline bool operator>=(const Edge& e) const { return _node >= e._node; }

    /**
     * Less than comparison operator for edges.
     * @param e The edge to compare to
     * @return True if this edge is less than e
     */
    inline bool operator< (const Edge& e) const { return _node <  e._node; }
    
    /**
     * Greater than comparison operator for edges.
     * @param e The edge to compare to
     * @return True if this edge is less than e
     */
    inline bool operator> (const Edge& e) const { return _node >  e._node; }

    /**
     * Negate this edge.
     */
    inline void negate()  { _node = (Node*) ((uintptr_t) _node ^ 1); }

    /**
     * Negate this edge if the condition is true.
     * @param negate If true, this edge will be negated.  If false, it
     * will remain the same.
     */
    inline void negateIf(bool negate) { assert(negate == (negate & 1)); _node = (Node*) ((uintptr_t) _node ^ negate); }

    /**
     * Take the "absolute value" of this edge -- make it positive.
     */
    inline void abs()     { _node = (Node*) ((uintptr_t) _node & ~1); }

    /**
     * Returns true if this edge is a NULL pointer.
     * @return true if this edge is a NULL pointer.
     */
    inline bool isNull()    const { return _node == NULL; }

    /**
     * Returns true if this edge points to a node.
     * @return true if this edge points to a node.
     */
    inline bool isNode()    const { return ((uintptr_t) _node & 2) == 0; }
    
    /**
     * Returns true if this edge points to a variable or constant.
     * @return true if this edge points to a variable or constant.
     */
    inline bool isVar()     const { return ((uintptr_t) _node & 2) == 2; }

    /**
     * Returns true if this edge is not negated.
     * @return true if this edge is not negated.
     */
    inline bool isPos()     const { return !((uintptr_t) _node & 1); }

    /**
     * Returns true if this edge is negated.
     * @return true if this edge is negated.
     */
    inline bool isNeg()     const { return  ((uintptr_t) _node & 1); }

    /**
     * Returns the negation of this edge.
     * @return the negation of this edge.
     */
    inline Edge operator~() const { return Edge((void*) ((uintptr_t) _node ^ 1)); }

    /**
     * Cast operator to convert this edge to an unsigned integer.
     * @return This edge represented as an unsigned integer.
     */
    inline operator uintptr_t()  const { return (uintptr_t) _node; }

    /**
     * Cast operator to convert this edge to a void pointer.
     * @return This edge represented as a void pointer.
     */
    inline operator void*()      const { return (void*)     _node; }

    /**
     * Return a pointer to the node that this edge points to.
     * @pre This edge points to a node and is not NULL.
     * @return The node that this edge points to.
     */
    inline Node*    getNodePtr() const { assert(isNode() && !isNull());  return  (Node*) ((uintptr_t) _node & ~3); }

    /**
     * Follow this edge to the node that it points to.
     * @pre This edge points to a node and is not NULL.
     * @return The node that this edge points to.
     */
    inline Node&    operator*()  const { assert(isNode() && !isNull());  return *(Node*) ((uintptr_t) _node & ~3); }

    /**
     * Follow this edge to the node that it points to.
     * @pre This edge points to a node and is not NULL.
     * @return The node that this edge points to.
     */
    inline Node*    operator->() const { assert(isNode() && !isNull());  return  (Node*) ((uintptr_t) _node & ~3); }
    
    /**
     * Follow this edge to the node that it points to.
     * @pre This edge points to a node and is not NULL.
     * @return The node that this edge points to.
     */
    inline Node&    getNode()    const { assert(isNode() && !isNull());  return *(Node*) ((uintptr_t) _node & ~3); }

    /**
     * Return the integer of the variable that this edge points to.
     * @pre This edge points to a variable or contsant.
     * @return The variable that this edge points to.
     */
    inline intptr_t getVar()     const { assert(!isNode()); return  ((intptr_t) _node) >> 2; }

    /**
     * Return an integer encoding the polarity of this edge.
     * @return 1 if the node is positive, or -1 if it is negative
     */
    inline intptr_t getPol()     const { return 1 - 2 * ((uintptr_t) _node & 1); }

    /**
     * Returns true if e points to the same thing in any polarity.
     * @param e The edge to compare to
     * @return True if e and this edge point to the same thing
     */
    inline bool sameNode(const Edge& e) const        { return ((((uintptr_t) _node) ^ ((uintptr_t) e._node)) & ~1) == 0; }

    /**
     * Return true if e and this edge have the same sign.
     * @param e The edge to compare to
     * @return True if e and this node have the same sign.
     */
    inline bool sameSign(const Edge& e) const        { return (((uintptr_t) _node) & 1) == (((uintptr_t) e._node) & 1); }

    /**
     * Return true if e and this edge point to the same thing with
     * different polarities
     * @param e The edge to compare to
     * @return True if e and this edge point to the same thing with
     * different polarities.
     */
    inline bool sameNodeOppSign(const Edge& e) const { return (((uintptr_t) _node) ^ ((uintptr_t) e._node)) == 1; }

    /**
     * Return true if this edge points to a constant (true or false)
     * @return true if this edge points to a constant
     */
    inline bool isConst() const { return ((uintptr_t) _node & ~1) == 2; }

    /**
     * Return true if this edge points to the constant true
     * @return true if this edge points to the constant true
     */
    inline bool isTrue()  const { return _node == (Node*) 2; }

    /**
     * Return true if this edge points to the constant false
     * @return true if this edge points to the constant false
     */
    inline bool isFalse() const { return _node == (Node*) 3; }
  };

  /**
   * Edge pointing to NULL.
   */
  static const Edge Null((Node*) NULL);

  /**
   * Edge pointing to True
   */
  static const Edge True(0, false);

  /**
   * Edge pointing to False.
   */
  static const Edge False(0, true);
};

#endif//NICESAT_ADTS_EDGE_H_
