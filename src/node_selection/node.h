#ifndef SBNB_NODE_SELECTION_NODE_H_
#define SBNB_NODE_SELECTION_NODE_H_

#include "core/optimization_problem.h"

class Node {
public:
  Node(OptimizationProblem* problem_, Node* parent = NULL, int level = 0);
  OptimizationProblem* problem;
  void SetFirstChild(Node*);
  void SetNextSibling(Node*);
  const Node* GetFirstChild() const;
  const Node* GetParent() const;
  const Node* GetNextSibling() const;
  const int GetLevel() const;

  void Fathom();
  const bool IsFathomed() const;

private:
  Node* parent_ = NULL;
  Node* sibling_ = NULL;
  Node* first_child_ = NULL;

  int level_ = 0;
  bool fathomed_ = false;

  const bool SiblingFathomed() const;
  const bool IsRoot() const;
};

#endif //SBNB_NODE_SELECTION_NODE_H_
