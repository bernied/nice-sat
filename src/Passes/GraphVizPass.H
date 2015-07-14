#ifndef NICESAT_PASSES_GRAPH_VIZ_PASS_H_
#define NICESAT_PASSES_GRAPH_VIZ_PASS_H_

#include "ADTs/Edge.H"
#include "ADTs/Vec.H"
#include "ADTs/HashTable.H"
#include "NiceSat.H"

#include <fstream>
#include <string>
using namespace std;

namespace nicesat {
  template<bool intAnnot = true, bool pexp = false>
  class GraphVizPass {
  public:
    GraphVizPass() : _nodeNum(0) { };
    ~GraphVizPass() { };

    void operator()(const char* filename, Edge e, Vec<Edge>& workQ);

  private:
    HashTable<Edge, int> _visited;
    int                  _nodeNum;

    int discoverNode(ofstream& out, Vec<Edge>& workQ, Edge e);
    void drawEdge(ofstream& out, Vec<Edge>& workQ,
                  int src, Edge dst, const char* color = "black");
  };

  template<bool intAnnot, bool pexp>
  void GraphVizPass<intAnnot, pexp>::drawEdge(ofstream& out, Vec<Edge>& workQ,
                                              int src, Edge dst, const char* color) {
    bool isNeg = dst.isNeg(); dst.abs();
    int dstNum = discoverNode(out, workQ, dst);;
    
    out << "  n" << src << " -> " << (dst.isVar() ? "v" : "n") << dstNum
        << "[arrowtail=" << (isNeg ? "dot" : "none")
        << ", color=" << color << "];\n";
  }

  template<bool intAnnot, bool pexp>
  int GraphVizPass<intAnnot, pexp>::discoverNode(ofstream& out, Vec<Edge>& workQ, Edge e) {
    assert(e.isPos());

    int ans = 0;
    if (!_visited.lookup(e, ans)) {
      if (e.isVar()) {
        ans = e.getVar();
        out << "  v" << e.getVar() << "[label=<Var " << e.getVar() << ">];\n";
      } else {
        ans = ++_nodeNum;
        out << "  n" << ans << "[label=<" << opString(e->op());
        if (intAnnot) out << " (" << e->intAnnot(false) << ", " << e->intAnnot(true) << ")"; 
        out << ">];\n";
        workQ.push(e);
      }
      _visited.insert(e, ans);
    }
    return ans;
  }

  template<bool intAnnot, bool pexp>
  void GraphVizPass<intAnnot, pexp>::operator()(const char* filename, Edge root, Vec<Edge>& workQ) {
    // open output file
    ofstream out((string(filename) + ".dot").c_str());
    out << "digraph nicedag {\n";
    
    if (root.isConst() || root.isVar()) {
      /// @TODO Output the constant or variable
      return;
    }

    // initialize workspace
    bool rootIsNeg = root.isNeg(); root.abs();
    workQ.clear(); discoverNode(out, workQ, root);

    // Actual DFS loop performing the graphviz pass
    while (workQ.size() != 0) {
      Edge e(workQ.last()); workQ.pop();
      
      int srcNum = 0;
      if (!_visited.lookup(e, srcNum)) errorf("Expected src to have been visited already");

      if (pexp && (e->op() == NodeOp_Ite || e->op() == NodeOp_Iff)) {
        if (e->ptrAnnot(false)) drawEdge(out, workQ, srcNum, e->ptrAnnot(false), "green");
        if (e->ptrAnnot(true))  drawEdge(out, workQ, srcNum, e->ptrAnnot(true), "red");
      } else {
        for (int i = 0; i< e->size(); i++) drawEdge(out, workQ, srcNum, (*e)[i]);
      }
    }

    // takes advantage of the fact that root is always first node (n1)
    out << "  root [shape=none, label=\"\"];\n"
        << "  root -> n1 [arrowtail=" << (rootIsNeg ? "dot" : "none") << "];\n"
        << "}\n";
  }
};

#endif//NICESAT_PASSES_GRAPH_VIZ_PASS_H_
