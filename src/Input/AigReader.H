#ifndef NICESAT_INPUT_AIG_READER_H_
#define NICESAT_INPUT_AIG_READER_H_

#include<stdio.h>
#include<string>

#include "ADTs/Edge.H"
#include "ADTs/Vec.H"
#include "Support/Reader.H"

namespace nicesat {
  class NiceSat;
  
  class AigReader {
  public:
    AigReader(const char* filename);
    ~AigReader();

    Edge read(NiceSat& sat,bool &status_ok);

  private:
    void readHeader(bool &status_ok);
    void readVariables(NiceSat& sat);
    Edge readOutputsAndNodes(NiceSat& sat);
    void readNodes(NiceSat& sat);
    
    Vec<Edge>  _nodes;
    Reader     _in;

    int           _maxVarIndex;
    int           _numInputs;
    int           _numLatches;
    int           _numOutputs;
    int           _numAndGates;
    
    Edge numToExp(int num) {
      Edge e = _nodes[num >> 1];
      if (e.isNull())
        errorf("Argument used before definition.  '%s' is an invalid AIG.", _in.getFileName());
      e.negateIf(num & 1);
      return e;
    }
  };
}

#endif//NICESAT_INPUT_AIG_READER_H_
