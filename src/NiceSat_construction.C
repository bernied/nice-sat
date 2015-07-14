#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "NiceSat.H"
#include "ADTs/Sort.H"

namespace nicesat {
  Edge NiceSat::createAnd(Vec<Edge>& args) {
    if (args.size() >  UINT16_MAX) 
      {printf("a gate has more than UNIT16_MAX arguments\n");
	exit(1);
	}
    sort(args);

    // Eliminate true, then check for trivial cases
    while (args.size() > 0 && args.first() == True) args.popFront();

    if (args.size() == 0) {
      return True;
    } else if (args.size() == 1) {
      return args[0];
    } else if (args.first() == False) {
      return False;
    }

    // Look for multiple copies (in any polarity) of any argument
    int outIdx = 0;
    for (size_t i = 1; i < args.size(); i++) {
      if (args[outIdx].sameNode(args[i])) {
        if (!args[outIdx].sameSign(args[i])) return False;
      } else {
        args[++outIdx] = args[i];
      }
    }
    
    args.shrinkBack(args.size() - outIdx - 1);
    if (args.size() == 1) {
      return args[0];
    }

    if (doMatching                  && args.size() == 2            &&
        args[0].isNeg()             && args[1].isNeg()             &&
        args[0].isNode()            && args[1].isNode()            &&
        args[0]->op() == NodeOp_And && args[1]->op() == NodeOp_And &&
        args[0]->size() == 2        && args[1]->size() == 2) {
      ++_numMatches;
      
      if ((*args[0])[0].sameNodeOppSign((*args[1])[0])) {
        return ~createIte((*args[0])[0], (*args[0])[1], (*args[1])[1]);
      } else if ((*args[0])[0].sameNodeOppSign((*args[1])[1])) {
        return ~createIte((*args[0])[0], (*args[0])[1], (*args[1])[0]);
      } else if ((*args[0])[1].sameNodeOppSign((*args[1])[0])) {
        return ~createIte((*args[0])[1], (*args[0])[0], (*args[1])[1]);
      } else if ((*args[0])[1].sameNodeOppSign((*args[1])[1])) {
        return ~createIte((*args[0])[1], (*args[0])[0], (*args[1])[0]);
      }
      
      --_numMatches;
    }

    return _gtab.uNode(NodeOp_And, args);
  }

  Edge NiceSat::createAnd(Edge a, Edge b) {
    if (a > b) { Edge t = b; b = a; a = t; }

    if (a.isConst()) {
      return (a.isTrue() ? b : False);
    } else if (a.sameNode(b)) {
      return (a.sameSign(b) ? b : False);
    }

    if (doMatching            &&
        a.isNeg()             && b.isNeg()             &&
        a.isNode()            && b.isNode()            &&
        a->op() == NodeOp_And && b->op() == NodeOp_And &&
        a->size() == 2        && b->size() == 2) {
      ++_numMatches;
      
      if ((*a)[0].sameNodeOppSign((*b)[0])) {
        return ~createIte((*a)[0], (*a)[1], (*b)[1]);
      } else if ((*a)[0].sameNodeOppSign((*b)[1])) {
        return ~createIte((*a)[0], (*a)[1], (*b)[0]);
      } else if ((*a)[1].sameNodeOppSign((*b)[0])) {
        return ~createIte((*a)[1], (*a)[0], (*b)[1]);
      } else if ((*a)[1].sameNodeOppSign((*b)[1])) {
        return ~createIte((*a)[1], (*a)[0], (*b)[0]);
      }
      
      --_numMatches;
    }

    return _gtab.uNode(NodeOp_And, a, b);
  }

  Edge NiceSat::createOr(Edge a, Edge b) {
    a.negate(); b.negate();
    Edge ans = createAnd(a, b);
    ans.negate();
    return ans;
  }

  Edge NiceSat::createOr(Vec<Edge>& args) {
    for(size_t i = 0; i < args.size(); i++) {
      args[i].negate();
    }
    Edge ans = createAnd(args);
    ans.negate();
    return ans;
  }

  Edge NiceSat::createImpl(Edge lhs, Edge rhs) {
    rhs.negate();
    Edge ans = createAnd(lhs, rhs);
    ans.negate();
    return ans;
  }
  
  Edge NiceSat::createIff(Edge lhs, Edge rhs) {
    bool negate = !lhs.sameSign(rhs);
    lhs.abs(); rhs.abs();

    Edge e;
    if (lhs == rhs) {
      e = True;
    } else if (lhs < rhs) {
      e = (lhs.isConst() ? rhs : _gtab.uNode(NodeOp_Iff, lhs, rhs));
    } else {
      e = (rhs.isConst() ? lhs : _gtab.uNode(NodeOp_Iff, rhs, lhs));
    }
    if (negate) e.negate();
    return e;
  }

  Edge NiceSat::createIte(Edge tst, Edge tt, Edge ff) {
    if (tst.isNeg()) {
      Edge tmp = tt; tt = ff; ff = tmp;
      tst.negate();
    }
    
    bool negate = tt.isNeg();
    if (negate) {
      tt.negate(); ff.negate();
    }

    Edge ans;
    if (tst == True) {
      ans = tt;
    } else if (tt == True || tst == tt) {
      ans = createOr(tst, ff);
    } else if (ff == True || tst.sameNodeOppSign(ff)) {
      ans = createImpl(tst, tt);
    } else if (ff == False || tst == ff) {
      ans = createAnd(tst, tt);
    } else if (tt == ff) {
      ans = tt;
    } else if (tt.sameNodeOppSign(ff)) {
      if (tst < tt) {
        ans = _gtab.uNode(NodeOp_Iff, tst, tt);
      } else {
        ans = _gtab.uNode(NodeOp_Iff, tt, tst);
      }
    } else {
      ans = _gtab.uNode(NodeOp_Ite, tst, tt, ff);
    }
    
    if (negate) ans.negate();
    return ans;
  }
}
