#include <stdio.h>
#include <string>

#include "Input/AigReader.H"
#include "NiceSat.H"

namespace nicesat {
  AigReader::AigReader(const char* filename) : _in(Reader(filename)) {
    // Intentionally Empty
  }
  
  AigReader::~AigReader() {
    // Intentionally Empty
  }

  Edge AigReader::read(NiceSat& sat,bool &status_ok) {
    status_ok = true;
    readHeader(status_ok);

    // Prepare the sat solver
    sat.getSolver().reserveLit(_numInputs);
    sat.predictNumNodes(_maxVarIndex); _nodes.clear(); _nodes.grow(_maxVarIndex + 1);

    // Read the variables
    readVariables(sat);

    // Read the AIG
    return readOutputsAndNodes(sat);
  }

  void AigReader::readHeader(bool &status_ok) {
    _in.readExpected("aig ");
    _maxVarIndex = _in.readNum(' ');
    _numInputs   = _in.readNum(' ');
    _numLatches  = _in.readNum(' ');
    _numOutputs  = _in.readNum(' ');
    _numAndGates = _in.readNum('\n');

    if (_numLatches != 0)
      {//printf("Latches unsupported, '%s' is an invalid AIG file\n", _in.getFileName());
       printf("'%s' is a sequential circuit (has latches)\n", _in.getFileName());
       printf("c To produce a combinational circuit go to directory '../nice-sat-0.2/aiger' and use the utility 'aigbmc'\n");
       status_ok = false;
       return;
       }
    
    if (_maxVarIndex != _numAndGates + _numLatches + _numInputs)
      {printf("Number of nodes don't add up.  '%s' is an invalid AIG file\n", _in.getFileName());
       status_ok = false;
       return;
       }
  }

  void AigReader::readNodes(NiceSat& sat) {
    unsigned int lhs, delta, rhs0;
    for (int i = _numInputs + 1; i <= _maxVarIndex; i++) {
      lhs = i << 1;
      delta = _in.readNumBinary();
      assert(delta <= lhs);
      rhs0 = lhs  - delta;
      
      delta = _in.readNumBinary();
      assert(delta <= rhs0);
      
      _nodes.push(sat.createAnd(numToExp(rhs0), numToExp(rhs0 - delta)));
    }
  }

  void AigReader::readVariables(NiceSat& sat) {
    // Create the variables
    _nodes.push(False);
    for (int i = 1; i <= _numInputs; i++) {
      Edge var = sat.createVar_(i);
      assert(var.isVar() && var.getVar() == i);
      _nodes.push(var);
    }
  }

  Edge AigReader::readOutputsAndNodes(NiceSat& sat) {
    // Record the outputs
    Vec<int> outputs(_numOutputs);
    for (int i = 0; i < _numOutputs; i++) outputs.push(_in.readNum("\n"));

    readNodes(sat); // read all the nodes

    // Create the output node
    Vec<Edge> args;
    for (int i = 0; i < _numOutputs; i++) args.push(numToExp(outputs[i]));
    Edge ans = sat.createAnd(args);

    assert(_nodes.size() == _maxVarIndex + 1); 
    return ans;
  }
}

