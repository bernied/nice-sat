#ifndef NICESAT_ADTS_NODE_H_
#define NICESAT_ADTS_NODE_H_

#include <stdint.h>

#include "ADTs/Vec.H"
#include "ADTs/HashTable.H"
#include "ADTs/Edge.H"

#define NODE_NUM_ANNOTS 6

namespace nicesat {
  /** 
   * Enumeration describing the operators that can appear on a node.
   */
  enum NodeOp {
    NodeOp_And,
    NodeOp_Ite,
    NodeOp_Iff
  };

  /**
   * Function returning the string corresponding to a given operator.
   * Mainly used in debugging/printing functions.
   */
  const char* opString(NodeOp op);

  /**
   * Structure that contains all of the 'small' bit flags that need to
   * be attached to a node.  Flags where polarity matters should have
   * two bits, one for positive and one for negative.
   *
   * Ideally, this should fit in 16 bits (so with _numArgs it takes up
   * 32-bits)
   */
  struct NodeBitData {  // ideally, this should fit in 16 bits
    /**
     * Two bits for the operator.
     */
    NodeOp _op:2;

    /**
     * One bit to record whether or not a variable is forced.
     */
    int    _varForced:1;

    /**
     * Whether or not the statistics pass has visited this node yet.
     */
    int    _statsVisited:1;

    /**
     * Whether or not the node has been pseudo-expanded (one bit for
     * each polarity).
     */
    int    _pexpVisited:2;

    /**
     * Whether or not the node has been "discovered" by the CNF pass
     * (visited in the down direction), one bit for each polarity
     */
    int    _cnfVisitedDown:2;

    /**
     * Whether or not the CNF pass has finished producing CNF for the
     * node (in the upward direction), one bit for each polarity
     */
    int    _cnfVisitedUp:2;

    /**
     * Constructor for the bit data structure.
     * @param op The operator for this node.
     */
    NodeBitData(NodeOp op) : _op(op), _varForced(0), _statsVisited(0), _pexpVisited(0),
                             _cnfVisitedDown(0), _cnfVisitedUp(0) { };
  };

  /**
   * This is the class used for representing nodes in the NICE dag.
   * Some effort is made to compact as much information into as few
   * actual bits as necessary. The public accessors provide support
   * for actually accessing the data in a clean manner.
   *
   * Many of the pieces of information associated with the node are
   * polarity aware.  That is different values are stored for the
   * positive and negative occurrences of the node.
   */
  class Node {
  public:
    /**
     * Special version of the new operator that requires the number of
     * arguments to be passed in.  Allocates extra space for the
     * variable length array at the end.
     * @param size The size of the node
     * @param nArgs The number of extra arguments to allocate space for
     */
    inline void* operator new(size_t size, size_t nArgs) {
      return (void*) new char[size + nArgs * sizeof(Edge)];
    }

    /**
     * Special verson of the delete operator, to match up with
     * definition of new.
     * @param node The node to be deleted
     */
    inline void operator delete(void* node) {
      return delete[] (char*) node;
    }

    /**
     * Constructor for a two-argument node.
     * @param op The operator
     * @param arg0 The first argument
     * @param arg1 The second argument
     */
    Node(NodeOp op, Edge arg0, Edge arg1) 
      : _bitData(op), _numArgs(2) {
      _intAnnot[0] = 0; _intAnnot[1] = 0; _ptrAnnot[0] = NULL; _ptrAnnot[1] = NULL;
      _args[0] = arg0; _args[1] = arg1;
    }

    /**
     * Constructor for a three-argument node.
     * @param op The operator
     * @param arg0 The first argument
     * @param arg1 The second argument
     * @param arg2 The third argument
     */
    Node(NodeOp op, Edge arg0, Edge arg1, Edge arg2)
      : _bitData(op), _numArgs(3) {
      _intAnnot[0] = 0; _intAnnot[1] = 0; _ptrAnnot[0] = NULL; _ptrAnnot[1] = NULL;
      _args[0] = arg0; _args[1] = arg1; _args[2] = arg2;
    }

    /**
     * Constructor for a higher-arity node.
     * @param op The operator
     * @param args A vector of the argumnets
     */
    Node(NodeOp op, const Vec<Edge>& args)
      : _bitData(op), _numArgs(args.size()) {
      _intAnnot[0] = 0; _intAnnot[1] = 0; _ptrAnnot[0] = NULL; _ptrAnnot[1] = NULL;
      memcpy(_args, args.c_array(), args.size() * sizeof(Edge));
    }

    /**
     * Destructor for the node.
     */
    ~Node() { };

    /**
     * Accessor for the operator.
     * @return The operator of this node
     */
    inline NodeOp   op()    const      { return (NodeOp) _bitData._op; }

    /**
     * Accessor for the number of arguments in this node
     * @return The size of this node.
     */
    inline size_t   size()  const      { return (size_t) _numArgs; }

    /**
     * Acessor for the arguments in this node.
     * @param idx The index to be read.
     * @return The edge containing the given argument
     */
    inline const Edge& operator[] (int idx) const { assert(idx < _numArgs); return _args[idx]; }

    /**
     * Equality operator for Nodes.  Performs a shallow comparison
     * (same operator, same size, edges are pointers to the same nodes)
     * @param other The node to compare to
     * @return True if the nodes are equal.
     */
    bool operator==(const Node& other) const;

    /**
     * Inequality operator for Nodes.  Performs a shallow comparison
     * (same operator, same size, edges are pointers to the same nodes)
     * @param other The node to compare to
     * @return True if the nodes are not equal.
     */
    bool operator!=(const Node& other) const;

    /**
     * Accessor for the integer annotation.
     * @param isNeg True if we want to access the negative annotation,
     * false otherwise.
     * @return The value of the integer annotation.
     */
    uint32_t  intAnnot(bool isNeg) const { assert(isNeg == (isNeg & 1)); return _intAnnot[(int) isNeg]; }

    /**
     * Accessor and modifier for the integer annotation.
     * @param isNeg True if we want to access the negative annotation,
     * false otherwise.
     * @return A reference to the integer annotation.
     */
    uint32_t& intAnnot(bool isNeg)       { assert(isNeg == (isNeg & 1)); return _intAnnot[(int) isNeg]; }

    /**
     * Accessor for the pointer annotation.
     * @param isNeg True if we want to access the negative annotation,
     * false otherwise.
     * @return The value of the pointer annotation.
     */
    void*     ptrAnnot(bool isNeg) const { assert(isNeg == (isNeg & 1)); return _ptrAnnot[(int) isNeg]; }

    /**
     * Accessor and modifier for the pointetr annotation.
     * @param isNeg True if we want to access the negative annotation,
     * false otherwise.
     * @return A reference to the pointer annotation.
     */
    void*&    ptrAnnot(bool isNeg)       { assert(isNeg == (isNeg & 1)); return _ptrAnnot[(int) isNeg]; }

    /**
     * Accessor for whether this node was visited by the statistics
     * pass.
     * @return True if the node has been visited.
     */
    bool wasStats()                const { return _bitData._statsVisited; }

    /**
     * Mark the node as having been visited by the statistics pass.
     */
    void visitStats()                    { _bitData._statsVisited = 1;    }

    /**
     * Accessor for whether a variable is being forced for this node.
     * @return True if this node has been marked to always receive a
     * proxy.
     */
    bool isVarForced()             const { return _bitData._varForced; }

    /**
     * Mark this node as always requiring a proxy.
     */
    void setVarForced()                  { _bitData._varForced = 1; }

    /**
     * Accessor for whether the node has been pseudo-expanded in the
     * given polarity.
     * @param isNeg True if interested in the negative polarity, false
     * otherwise.
     * @return Whether or not the node has been expanded in the given polarity
     */
    bool wasExpanded(bool isNeg)   const { assert((isNeg & 1) == isNeg); return _bitData._pexpVisited & (1 << isNeg); }

    /**
     * Mark the node as having been pseudo-expanded in the given polarity.
     * @param isNeg True if interested in the negative polarity, false
     * otherwise.
     */
    void setExpanded(bool isNeg)         { assert((isNeg & 1) == isNeg); _bitData._pexpVisited |= (1 << isNeg); }

    /**
     * Accessor for whether the node has been discovered by the CNF
     * generation algorithm in the given polarity.
     * @param isNeg True if interested in the negative polarity, false
     * otherwise.
     * @return Whether or not the node has been discovered by CNF generation.
     */
    bool wasCNFedDown(bool isNeg)  const { assert((isNeg & 1) == isNeg); return _bitData._cnfVisitedDown & (1 << isNeg); }

    /**
     * Mark the node as having been discovered by CNF generation in
     * the given polarity
     * @param isNeg True if interested in the negative polarity, false
     * otherwise.
     */
    void setCNFedDown(bool isNeg)        { assert((isNeg & 1) == isNeg); _bitData._cnfVisitedDown |= (1 << isNeg); }

    /**
     * Accessor for whether the node has been finished by the CNF
     * generation algorithm in the given polarity.
     * @param isNeg True if interested in the negative polarity, false
     * otherwise.
     * @return Whether or not the node has been finished by CNF generation.
     */
    bool wasCNFedUp(bool isNeg)    const { assert((isNeg & 1) == isNeg); return _bitData._cnfVisitedUp & (1 << isNeg); }

    /**
     * Mark the node as having had CNF generated for it in the given
     * polarity
     * @param isNeg True if interested in the negative polarity, false
     * otherwise.
     */
    void setCNFedUp(bool isNeg)          { assert((isNeg & 1) == isNeg); _bitData._cnfVisitedUp |= (1 << isNeg); }

  private:
    /**
     * Details about the node that only need a few bits, packed into a
     * single structure.  Include operator, and various visited flags.
     */
    NodeBitData _bitData;

    /**
     * 16-bit integer to store the number of arguments to the node.
     */
    uint16_t    _numArgs;

    /**
     * One positive/one negative 32-bit integer annotation.
     */
    uint32_t _intAnnot[2];

    /**
     * One positive/one negative 32/64-bit pointer annotation.
     */
    void*    _ptrAnnot[2];

    /**
     * The variable length array of arguments.  By keeping this right
     * after the node in memory, we increase locality.
     * NOTE: This must appear as the last field in the class so that
     * the allocation functions work correctly.
     */
    Edge _args[0];

    /**
     * Prohibit calling "regular" new on a node (must specify th
     * enumber of arguments to allocate the variable length array).
     * @param size The size (in bytes) to allocate
     */
    void* operator new(size_t size) { errorf("Called regular new on Node!"); return new unsigned char[size]; }

    /**
     * Prohibit calling the copy constructor -- it is inefficient, and
     * shouldn't be necessary since the global table requires we pass
     * everything by pointer.
     * @param other The node to copy
     */
    Node(const Node& other) : _bitData(NodeOp_And) { errorf("Called copy constructor on Node!"); };

    /**
     * Assignment operator.  As wit the copy constructor it shouldn't
     * be used for efficiency reasons, and because the global table
     * makes it unnecessary.
     * @param other The node to copy
     */
    const Node& operator=(const Node& other) { errorf("Called assignment operator on Node!"); return *this; };
  };
};

#endif//NICESAT_ADTS_NODE_H_
