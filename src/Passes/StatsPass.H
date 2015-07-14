#ifndef NICESAT_PASSES_STATS_PASS_H_
#define NICESAT_PASSES_STATS_PASS_H_

#include "ADTs/Edge.H"
#include "ADTs/Vec.H"

namespace nicesat {
  class StatsPass {
  public:
    StatsPass() : _maxAndOutDegree(0), _maxAtom(0), _numIteNodes(0), _numIffNodes(0), _numAndNodes(0) { }
    ~StatsPass() { }
    
    void operator()(Edge root, Vec<Edge>& workQ);
    void display() const;
    
  private:
    int _maxAndOutDegree;
    int _maxAtom;
    
    int _numIteNodes;
    int _numIffNodes;
    int _numAndNodes;
  };
}

#endif//NICESAT_PASSES_STATS_PASS_H_
