#include "Passes/CnfPass.H"
#include "ADTs/CnfExp.H"
#include "ADTs/Node.H"

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define POLARITY
#endif

namespace nicesat {
  CnfExp* CnfPass::fillArgs(Edge e, bool isNeg, Edge& largestEdge) {
    assert(e.isNode() && e->op() == NodeOp_And);
    assert(e->wasCNFedDown(isNeg));
    _args.clear();

    largestEdge = (void*) NULL;
    CnfExp* largest = NULL;
    int i = e->size();
    while (i != 0) {
      Edge arg = (*e)[--i]; arg.negateIf(isNeg);
      assert(!arg.isNull());

      if (arg.isVar()) {
        _args.push(arg);
        continue;
      }

      if (arg->op() == NodeOp_Ite || arg->op() == NodeOp_Iff) {
        assert(arg->wasExpanded(arg.isNeg()));
        arg = arg->ptrAnnot(arg.isNeg());
      }
      
      assert(arg->op() != NodeOp_Ite && arg->op() != NodeOp_Iff);
      assert(arg->wasCNFedUp(arg.isNeg()));
      assert(arg->intAnnot(arg.isNeg()) > 0);

      assert(!arg.isVar());
      if (arg->intAnnot(arg.isNeg()) == 1) {
        CnfExp* argExp = (CnfExp*) arg->ptrAnnot(arg.isNeg());
        assert(argExp != NULL);
        if (!isProxy(argExp)) {
          if (largest == NULL) {
            largest = argExp;
            largestEdge = arg;
            continue;
          } else if (argExp->litSize() > largest->litSize()) {
            _args.push(largestEdge);
            largest = argExp;
            largestEdge = arg;
            continue;
          }
        }
      }
      _args.push(arg);
    }

    if (largest != NULL) {
      assert(--largestEdge->intAnnot(largestEdge.isNeg()) == 0);
      largestEdge->ptrAnnot(largestEdge.isNeg()) = NULL;
      assert(_args.size() == 1);
    } else {
      assert(_args.size() == 2);
    }

    return largest;
  }

  void CnfPass::saveCNF(Solver& solver, CnfExp* exp, Edge e, bool sign) {
    assert(!e->wasCNFedUp(sign));
    e->setCNFedUp(sign);
    if (exp == NULL || isProxy(exp)) return;
    
    if (exp->litSize() == 1) {
      Literal l = exp->singletons()[0];
      delete exp;
      e->ptrAnnot(sign) = (void*) ((intptr_t) (l << 1) | 1);
    } else if (exp->litSize() != 0 && (e->intAnnot(sign) > 1 || e->isVarForced())) {
      introProxy(solver, e, exp, sign);
    } else {
      e->ptrAnnot(sign) = exp;
    }
  }

  bool CnfPass::propagate(Solver& solver, CnfExp*& dest, CnfExp* src, bool negate) {
    if (src != NULL && !isProxy(src) && src->litSize() == 0) {
      assert(src->alwaysFalse() || src->alwaysTrue());
      
      if (dest == NULL) {
        dest = new CnfExp(negate ? src->alwaysFalse() : src->alwaysTrue());
      } else if (isProxy(dest)) {
        bool alwaysTrue = (negate ? src->alwaysFalse() : src->alwaysTrue());
        if (alwaysTrue) {
          solver.add(getProxy(dest));
        } else {
          solver.add(-getProxy(dest));
        }

        dest = new CnfExp(negate ? src->alwaysFalse() : src->alwaysTrue());
      } else {
        dest->clear(negate ? src->alwaysFalse() : src->alwaysTrue());
      }
      return true;
    }
    return false;
  }
   
  void CnfPass::produceCNF(Solver& solver, Edge e) {
    CnfExp* expPos = NULL;
    CnfExp* expNeg = NULL;

    // produce expPos if necessary
    if (expPos == NULL && e->intAnnot(false) > 0) {
      assert(e->wasCNFedDown(false));
      assert(!e->wasCNFedUp(false));
      expPos = produceConjunction(solver, e);
    }

    // produce expNeg if necessary
    if (expNeg == NULL && e->intAnnot(true)  > 0) {
      assert(e->wasCNFedDown(true));
      assert(!e->wasCNFedUp(true));
      expNeg = produceDisjunction(solver, e);
    }

    /// @todo Propagate constants across semantic negations (this can
    /// be done similarly to the calls to propagate shown below).  The
    /// trick here is that we need to figure out how to get the
    /// semantic negation pointers, and ensure that they can have CNF
    /// produced for them at the right point
    ///
    /// propagate(solver, expPos, snPos, false) || propagate(solver, expNeg, snNeg, false)

    // propagate from positive to negative, negative to positive
    propagate(solver, expPos, expNeg, true) || propagate(solver, expNeg, expPos, true);

#ifdef POLARITY
    // The polarity heuristic entails visiting the discovery polarity first
    if (e.isPos()) {
      saveCNF(solver, expPos, e, false);
      saveCNF(solver, expNeg, e, true);
    } else {
      saveCNF(solver, expNeg, e, true);
      saveCNF(solver, expPos, e, false);
    }
#else
    saveCnf(solver, expPos, e, false);
    saveCNF(solver, expNeg, e, true);
#endif
  }
  
  void CnfPass::operator()(Solver& solver, Edge root, bool backtrackLit, Vec<Edge>& workQ) {
    // Find the 'real' root
    if (root.isNode() && (root->op() == NodeOp_Ite || root->op() == NodeOp_Iff)) {
      root = root->ptrAnnot(root.isNeg());
      assert(root->op() != NodeOp_Ite && root->op() != NodeOp_Ite);
      assert(root.isPos());
    }

    // if it's "trivial", deal with it
    if (root.isConst()){
      if (root.isNeg()) {
        Literal l = solver.getNewVar();
        solver.add(l); solver.add(-l);
      }
      return;
    } else if (root.isVar()) {
      solver.add(atomLit(root));
      return;
    }
    
    // Initialize the workspace and prepare visited bits for the pass
    workQ.clear(); workQ.push(root);

    // Perform the DFS
    while (workQ.size() != 0) {
      Edge e(workQ.last());
      if (e.isNull() || e.isVar()) {
        workQ.pop();
        continue;
      } else if (e->op() == NodeOp_Ite || e->op() == NodeOp_Iff) {
        workQ.pop();
        workQ.push(e->ptrAnnot(false));
        workQ.push(e->ptrAnnot(true));
        continue;
      }

      bool needPos = (e->intAnnot(false) > 0);
      bool needNeg = (e->intAnnot(true)  > 0);

      if ((!needPos || e->wasCNFedUp(false)) &&
          (!needNeg || e->wasCNFedUp(true))) {
        workQ.pop();
      } else if ((needPos && !e->wasCNFedDown(false)) ||
                 (needNeg && !e->wasCNFedDown(true))) {
        if (needPos) e->setCNFedDown(false);
        if (needNeg) e->setCNFedDown(true);
        
        for (int i = 0; i < e->size(); i++) {
          Edge arg((*e)[i]); arg.negateIf(e.isNeg()); workQ.push((*e)[i]);
        }
      } else {
        assert(!needPos || e->wasCNFedDown(false));
        assert(!needNeg || e->wasCNFedDown(true));
        
        workQ.pop();
        produceCNF(solver, e);
      }          
    }

    // emit CNF for the root node
    assert(root->wasCNFedUp(root.isNeg()));
    assert(root->intAnnot(root.isNeg()) > 0);
    CnfExp* rootExp = (CnfExp*) root->ptrAnnot(root.isNeg());
    assert(rootExp);
    if (isProxy(rootExp)) {
      solver.add(getProxy(rootExp));
    } else if (backtrackLit) {
      solver.add(introProxy(solver, root, rootExp, root.isNeg()));
    } else {
      solver.add(*rootExp);
    }

    if (((intptr_t) rootExp & 1) == 0) {
      delete rootExp;
      root->ptrAnnot(root.isNeg()) = NULL;
    }
    assert(--root->intAnnot(root.isNeg()) == 0);
  }

  Literal CnfPass::introProxy(Solver& solver, Edge e, CnfExp* exp, bool isNeg) {
    assert(exp);
    assert(!((intptr_t) exp & 1));
    
    Literal l = 0;

    // This complicate bit of code is necessary to see if we can avoid
    // getting a new variable.  We first check teh negation of this
    // node, and if that fails we then look at the semantic negation
    // pointer.
    if (e->wasCNFedUp(!isNeg)) { // check the negation of this node
      CnfExp* otherExp = (CnfExp*) e->ptrAnnot(!isNeg);
      if (isProxy(otherExp)) l = -getProxy(otherExp);
    } else {                        // check the semantic negation pointer
      Edge semNeg(e->ptrAnnot(isNeg));
      if (!semNeg.isNull()) {
        if (semNeg->wasCNFedUp(isNeg)) {
          CnfExp* otherExp = (CnfExp*) semNeg->ptrAnnot(isNeg);
          if (isProxy(otherExp)) l = -getProxy(otherExp);
        } else if (semNeg->wasCNFedUp(!isNeg)) {
          CnfExp* otherExp = (CnfExp*) semNeg->ptrAnnot(!isNeg);
          if (isProxy(otherExp)) l = getProxy(otherExp);
        }
      }
    }

    // In this case, we haven't found an applicable proxy, so we get a
    // new variable
    if (l == 0) l = solver.getNewVar();

    // Output the constraints on the auxiliary variable
    solver.constrain(l, *exp); delete exp;
    
    // Record the variable that was introduced
    e->ptrAnnot(isNeg) = (void*) ((intptr_t) (l << 1) | 1);
    assert(isProxy((CnfExp*) e->ptrAnnot(isNeg)));
    assert(getProxy((CnfExp*) e->ptrAnnot(isNeg)) == l);

    return l;
  }
}
