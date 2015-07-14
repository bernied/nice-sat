#include "Passes/StatsPass.H"

#include "ADTs/Edge.H"
#include "ADTs/Node.H"

namespace nicesat {
  void StatsPass::operator()(Edge root, Vec<Edge>& workQ) {
    // If constant or variable, nothing to be done
    if (root.isConst() || root.isVar()) return;
    
    // Initialize the workspace and prepare visited bits for the pass
    workQ.clear(); workQ.push(root);

    while (workQ.size() != 0) {
      Edge e(workQ.last());
      if (e->wasStats()) { // already factored into stats
        workQ.pop();
      } else {
        e->visitStats();
        switch (e->op()) {
          case NodeOp_Ite: ++_numIteNodes; break;
          case NodeOp_Iff: ++_numIffNodes; break;
          case NodeOp_And: ++_numAndNodes;
            if (_maxAndOutDegree < e->size()) _maxAndOutDegree = e->size();
            break;
        }

        for (int i = 0; i < e->size(); i++) {
          Edge succ((*e)[i]);
          if (succ.isVar()) {
            if (succ.getVar() > _maxAtom) _maxAtom = succ.getVar();
          } else if (!succ->wasStats()) {
            workQ.push(succ);
          }
        }
      }
    }
  }

  void StatsPass::display() const {
    reportf("Statistics:\n  %-23s: %d\n  %-23s: %d\n  %-23s: %d\n"
                         "  %-23s: %d\n  %-23s: %d\n\n",
            "# ITE Nodes", _numIteNodes, "# IFF Nodes", _numIffNodes,
            "# AND Nodes", _numAndNodes, "Max Atom #",  _maxAtom,
            "Max AND Out Degree", _maxAndOutDegree);
  }
}
  
