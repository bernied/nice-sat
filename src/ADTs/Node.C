#include "ADTs/Node.H"
#include "Support/Shared.H"

#include <string.h>

namespace nicesat {
  const char* opString(NodeOp op) {
    switch (op) {
      case NodeOp_And: return "and";
      case NodeOp_Ite: return "ite";
      case NodeOp_Iff: return "iff";
    }
    return "unexpected op";
  }

  
  bool Node::operator==(const Node& other) const {
    if ((_bitData._op) != (other._bitData._op)) return false;
    if (_numArgs            != other._numArgs)  return false;
    for (size_t i = 0; i < size(); i++)
      if (operator[](i) != other[i]) 
        return false;
    return true;
  }

  bool Node::operator!=(const Node& other) const {
    if ((_bitData._op) != (other._bitData._op)) return true;
    if (_numArgs            != other._numArgs)  return true;
    for (size_t i = 0; i < size(); i++)
      if (operator[](i) != other[i]) 
        return true;
    return false;
  }
};
