#ifndef NICESAT_NAMED_NICE_SAT_H_
#define NICESAT_NAMED_NICE_SAT_H_

#include "NiceSat.H"
#include "ADTs/HashTable.H"

namespace nicesat {
  /**
   * A version of NiceSat that has been augmented with hash tables for
   * maintaining a variable<->name mapping.  Intended for use with
   * ASCII formats and in other situations where it is desirable to
   * have some information associated with the variables.
   */
  template <class Name, class H = Hash<Name>, class E = Equal<Name> >
  class NamedNiceSat : public NiceSat {
  private:
    /**
     * Hash table mapping variables (integers) to names.  Uses a hash
     * table because the variable range is not necessarily dense (in
     * fact, if used incrementally it will have large gaps
     * corresponding to CNF proxy variables).
     */
    HashTable<int, Name>        _varToName;

    /**
     * Hash table mapping names to variables (represented as edges)
     */
    HashTable<Name, Edge, H, E> _nameToVar;

  public:
    /**
     * Function to lookup the name.
     * @param var The edge to lookup.
     * @param name The name corresponding to that variable, returned
     * by reference if found.
     * @return Returns true if the variable was found, in which case
     * name has been modified.  False if the variable didn't have a
     * name.
     */
    bool lookupName(Edge var, Name& name) const;

    using NiceSat::createVar;

    /**
     * Version of createVar that uses a hash table to map names to
     * variables.  Potentially useful if dealing with an ascii format.
     * @param n The name of the variable
     * @return Edge corresponding to the variable
     */
    Edge createVar(const Name& n);

    /**
     * Initialized the hash tables to handle at least numNamedVars.
     * @param numNamedVars The expected number of named variables.
     */
    void predictNumNamedVars(size_t numNamedVars) {
      _varToName.grow(numNamedVars); _nameToVar.growTo(numNamedVars);
    }

    NamedNiceSat() : NiceSat(), _varToName(), _nameToVar() { }
    virtual ~NamedNiceSat() { };
  };

  template <class Name, class H, class E>
  bool NamedNiceSat<Name, H, E>::lookupName(Edge var, Name& name) const {
    if (!var.isVar())                   return false;
    return _varToName.lookup ((int) var.getVar(), name);
  }

  template <class Name, class H, class E>
  Edge NamedNiceSat<Name, H, E>::createVar(const Name& n) {
    Edge ans;
    if (!_nameToVar.lookup(n, ans)) {
      ans = createVar();
      _varToName.insert((int) ans.getVar(), n);
      _nameToVar.insert(n, ans);
    }
    return ans;
  }
}

#endif//NICESAT_NAMED_NICE_SAT_H_
