#ifndef NICESAT_PASSES_COUNT_AND_EXPAND_PASS_H_
#define NICESAT_PASSES_COUNT_AND_EXPAND_PASS_H_

#include "ADTs/Edge.H"
#include "ADTs/Vec.H"
#include "NiceSat.H"

namespace nicesat {
  class CountAndExpandPass {
  public:
    CountAndExpandPass() { };
    ~CountAndExpandPass() { };

    void operator()(NiceSat& sat, Edge e, Vec<Edge>& workQ);
  };
};

#endif//NICESAT_PASSES_COUNT_AND_EXPAND_PASS_H_
