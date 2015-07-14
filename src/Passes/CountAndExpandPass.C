#include "Passes/CountAndExpandPass.H"
#include "NiceSat.H"

namespace nicesat {
  void CountAndExpandPass::operator()(NiceSat& sat, Edge root,
                                      Vec<Edge>& workQ) {
    // If constant or variable, nothing to be done
    if (root.isConst() || root.isVar()) return;
    
    // Initialize the workspace and prepare visited bits for the pass
    workQ.clear(); workQ.push(root);

    bool wasMatchingOn = sat.doMatching;

    while (workQ.size() != 0) {
      Edge e(workQ.last()); workQ.pop();
      bool ePol = e.isNeg();

      if (e->wasExpanded(ePol)) {
        // if already visited, increment count
        if (e->op() == NodeOp_Iff || e->op() == NodeOp_Ite) {
          // increment the pseudo-expansion of the Iff or Ite
          Edge pExp(e->ptrAnnot(ePol));
          assert(pExp->wasExpanded(false));
          ++pExp->intAnnot(false);
        } else {
          ++e->intAnnot(ePol);
        }
      } else {
        // if not visited, pseudo-expand
        e->setExpanded(ePol); e->intAnnot(ePol) = 1;

        if (e->op() == NodeOp_Ite || e->op() == NodeOp_Iff) {
          e->intAnnot(ePol) = 0;
          
          Edge tst((*e)[0]); Edge tt((*e)[1]); Edge ff(e->op() == NodeOp_Iff ? ~tt : (*e)[2]);
          tt.negateIf(!ePol); ff.negateIf(!ePol);
          tt = sat.createAnd(tst, tt);
          tst.negate();
          ff = sat.createAnd(tst, ff);
          tt.negate(); ff.negate();
          sat.doMatching = false;
          Edge succ1((void*) sat.createAnd(tt, ff)); // succ1 is the pseudo-expansion in this polarity
          e->ptrAnnot(ePol) = (void*) succ1;
          sat.doMatching = wasMatchingOn;
          assert(!succ1.isVar());
          workQ.push(succ1); // need to push succ1 to ensure it gets visited

          // Set up the semantic negation pointers.  These are used
          // during CNF generation to enable the positive and negative
          // pseudo-expansions to constrain the same variable, even
          // though syntactically distinct
          if (e->wasExpanded(!ePol)) {
            Edge succ2((void*) e->ptrAnnot(!ePol)); // succ2 is the expansion in the opposite polarity
            assert((void*) succ2 != NULL);
            assert(succ1->ptrAnnot(false) == NULL);
            assert(succ2->ptrAnnot(false) == NULL);
            
            succ1->ptrAnnot(false) = (void*) succ2;
            succ2->ptrAnnot(false) = (void*) succ1;
            succ1->ptrAnnot(true)  = (void*) succ2;
            succ2->ptrAnnot(true)  = (void*) succ1;
          }
        } else {
          for (int i = 0; i < e->size(); i++) {
            Edge succ((*e)[i]); succ.negateIf(ePol);
            if (!succ.isVar()) workQ.push(succ);
          }
        }
      }
    }
  }
}
