#ifndef SBNB_NODE_SELECTION_NODE_SELECTION_H_
#define SBNB_NODE_SELECTION_NODE_SELECTION_H_

#include <queue>
#include "node.h"

class NodeSelection {

 public:
  virtual Node* NextNode() = 0;
  virtual Node* CurrentNode() const = 0;
  virtual void AddNode(Node* node) = 0;
  virtual bool HasNextNode() const = 0;
};

#endif //SBNB_NODE_SELECTION_NODE_SELECTION_H_
