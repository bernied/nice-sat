#include "NiceSat.H"
#include "Support/Shared.H"
#include <cstring>

#include "Passes/StatsPass.H"
#include "Passes/CountAndExpandPass.H"
#include "Passes/CnfPass.H"
#include "Passes/GraphVizPass.H"

namespace nicesat {
  NiceSat::NiceSat() :
    doMatching(true), doGraphViz(false), _time(0),
    _solver(new NullSolver()), _freeSolver(true) { }
                       
  NiceSat::NiceSat(Solver& solver) :
    doMatching(true), doGraphViz(false), _time(0),
    _solver(&solver), _freeSolver(false) { }
  
  void NiceSat::predictNumNodes(size_t numNodes) {
    _gtab.grow(numNodes);
  }

  void NiceSat::logStart(const char* fmt, ...) {
    assert(_time == 0);
    va_list args;
    va_start(args, fmt);
    char* msg = vnsprintf(fmt, args);
    va_end(args);
    
    _startMsgLen = strlen(msg) + 3;
    reportf("%s...", msg);
    free(msg);
    _time = cpuTime();
  }

  void NiceSat::logEnd() {
    _time = cpuTime() - _time;
    char buffer[80];
    if (_startMsgLen > 55) {
      _startMsgLen = 0;
      reportf("\n");
    }
    sprintf(buffer, "%%-%ds finished in %%.4g s (CPU)\n", 60 - _startMsgLen);
    reportf(buffer, "", _time);
    _time = 0;
  }

  void NiceSat::printStats() {
    reportf("Counts from NiceSat:\n  %-23s: %d\n\n",
            "# Matches", _numMatches);
  }

  void NiceSat::printGraphStats(Edge root) {
    logStart("Gathering statistics");
    _stats(root, _workQ);
    logEnd();
    _stats.display();
  }

  void NiceSat::add(Edge root, bool backtrackLit) {
    if (doGraphViz) GraphVizPass<false, false>()("nicedag-pre-exp", root, _workQ);
    
    {
      CountAndExpandPass expand;
      logStart("Counting shares and pseudo-expanding");
      expand(*this, root, _workQ);
      logEnd();
    }

    if (doGraphViz) GraphVizPass<true, true>()("nicedag-post-exp", root, _workQ);

    {
      CnfPass cnf;
      logStart("Generating CNF");
      cnf(*_solver, root, backtrackLit, _workQ);
      logEnd();
    }
  }

  Assignment NiceSat::deref(Edge e) const {
    if (e.isNode()) {
      if (e->op() == NodeOp_Iff || e->op() == NodeOp_Ite) {
        e = e->ptrAnnot(e.isNeg());
        assert((void*) e != NULL);
        assert(e.isPos());
        assert(e->op() != NodeOp_Iff && e->op() != NodeOp_Ite);
      }
      if (!e->wasCNFedUp(e.isNeg())) return Assignment_Unknown;
      if (e.isNeg()) {
        for (int i = 0; i < e->size(); i++) {
          Assignment ans = deref(~(*e)[i]);
          if (ans != Assignment_False) {
            return ans;
          }
        }
        return Assignment_False;
      } else {
        for (int i = 0; i < e->size(); i++) {
          Assignment ans = deref((*e)[i]);
          if (ans != Assignment_True) {
            return ans;
          }
        }
        return Assignment_True;
      }
    } else {
      assert(e.isVar() || e.isConst());
      Assignment ans = (e.isVar()
                        ? _solver->deref((Literal) e.getVar())
                        : Assignment_True);
      if (e.isNeg()) ans = (Assignment) ((int) ans * -1);
      return ans;
    }
  }
}
